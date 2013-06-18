/*
 * CCEGLViewlinux.cpp
 *
 *  Created on: Aug 8, 2011
 *      Author: laschweinski
 */

#include "CCEGLView.h"
#include "CCGL.h"
#include "ccMacros.h"
#include "CCDirector.h"
#include "touch_dispatcher/CCTouch.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "text_input_node/CCIMEDispatcher.h"

PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;

PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;

bool initExtensions() {
#define LOAD_EXTENSION_FUNCTION(TYPE, FN)  FN = (TYPE)glfwGetProcAddress(#FN);
	bool bRet = false;
	do {

		/* Supports frame buffer? */
		if (glfwExtensionSupported("GL_EXT_framebuffer_object") != GL_FALSE)
		{

			/* Loads frame buffer extension functions */
			LOAD_EXTENSION_FUNCTION(PFNGLGENERATEMIPMAPEXTPROC,
					glGenerateMipmapEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLGENFRAMEBUFFERSEXTPROC,
					glGenFramebuffersEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLDELETEFRAMEBUFFERSEXTPROC,
					glDeleteFramebuffersEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLBINDFRAMEBUFFEREXTPROC,
					glBindFramebufferEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC,
					glCheckFramebufferStatusEXT);
			LOAD_EXTENSION_FUNCTION(PFNGLFRAMEBUFFERTEXTURE2DEXTPROC,
					glFramebufferTexture2DEXT);

		} else {
			break;
		}

		if (glfwExtensionSupported("GL_ARB_vertex_buffer_object") != GL_FALSE) {
			LOAD_EXTENSION_FUNCTION(PFNGLGENBUFFERSARBPROC, glGenBuffersARB);
			LOAD_EXTENSION_FUNCTION(PFNGLBINDBUFFERARBPROC, glBindBufferARB);
			LOAD_EXTENSION_FUNCTION(PFNGLBUFFERDATAARBPROC, glBufferDataARB);
			LOAD_EXTENSION_FUNCTION(PFNGLBUFFERSUBDATAARBPROC,
					glBufferSubDataARB);
			LOAD_EXTENSION_FUNCTION(PFNGLDELETEBUFFERSARBPROC,
					glDeleteBuffersARB);
		} else {
			break;
		}
		bRet = true;
	} while (0);
	return bRet;
}

NS_CC_BEGIN

CCEGLView::CCEGLView()
: bIsInit(false)
, m_fFrameZoomFactor(1.0f)
{
}

CCEGLView::~CCEGLView()
{
}

void keyEventHandle(GLFWwindow* p_window,int iKeyID,int iScanCode,int iKeyState,int iMods) {
	if (iKeyState ==GLFW_RELEASE) {
		return;
	}

	if (iKeyID == GLFW_KEY_DELETE) {
		CCIMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
	} else if (iKeyID == GLFW_KEY_ENTER) {
		CCIMEDispatcher::sharedDispatcher()->dispatchInsertText("\n", 1);
	} else if (iKeyID == GLFW_KEY_TAB) {

	}
}

void charEventHandle(GLFWwindow* p_window, unsigned int uiCharID) {
	CCIMEDispatcher::sharedDispatcher()->dispatchInsertText((const char *)&uiCharID, 1);
}

void mouseButtonEventHandle(GLFWwindow* p_window,int iMouseID,int iMouseState,int iMods) {
	if (iMouseID == GLFW_MOUSE_BUTTON_LEFT) {
        CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
		//get current mouse pos
		double x,y;
		glfwGetCursorPos(p_window, &x, &y);
		CCPoint oPoint(x,y);
		/*
		if (!CCRect::CCRectContainsPoint(s_pMainWindow->m_rcViewPort,oPoint))
		{
			CCLOG("not in the viewport");
			return;
		}
		*/
         oPoint.x /= pEGLView->m_fFrameZoomFactor;
         oPoint.y /= pEGLView->m_fFrameZoomFactor;
		int id = 0;
		if (iMouseState == GLFW_PRESS) {
			pEGLView->handleTouchesBegin(1, &id, &oPoint.x, &oPoint.y);

		} else if (iMouseState == GLFW_RELEASE) {
			pEGLView->handleTouchesEnd(1, &id, &oPoint.x, &oPoint.y);
		}
	}
}

void cursorPosEventHandle(GLFWwindow* p_window,double dPosX,double dPosY) {
	int iButtonState = glfwGetMouseButton(p_window, GLFW_MOUSE_BUTTON_LEFT);

	//to test move
	if (iButtonState == GLFW_PRESS) {
            CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
            int id = 0;
            float x = dPosX;
            float y = dPosY;
            x /= pEGLView->m_fFrameZoomFactor;
            y /= pEGLView->m_fFrameZoomFactor;
            pEGLView->handleTouchesMove(1, &id, &x, &y);
	}
}

void closeEventHandle(GLFWwindow* p_window) {
	CCDirector::sharedDirector()->end();
}

void CCEGLView::setFrameSize(float width, float height)
{
	bool eResult = false;

	//check
	CCAssert(width!=0&&height!=0, "invalid window's size equal 0");

	//Inits GLFW
	eResult = glfwInit() != GL_FALSE;

	if (!eResult) {
		CCAssert(0, "fail to init the glfw");
	}

	/* Updates window hint */
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	int iDepth = 16; // set default value
	/* Depending on video depth */
	switch(iDepth)
	{
		/* 16-bit */
		case 16:
		{
            glfwWindowHint(GLFW_RED_BITS, 5);
            glfwWindowHint(GLFW_GREEN_BITS, 6);
            glfwWindowHint(GLFW_BLUE_BITS, 5);
            glfwWindowHint(GLFW_ALPHA_BITS, 0);
            glfwWindowHint(GLFW_DEPTH_BITS, 16);
			break;
		}

		/* 24-bit */
		case 24:
		{
            glfwWindowHint(GLFW_RED_BITS, 8);
            glfwWindowHint(GLFW_GREEN_BITS, 8);
            glfwWindowHint(GLFW_BLUE_BITS, 8);
            glfwWindowHint(GLFW_ALPHA_BITS, 0);
            glfwWindowHint(GLFW_DEPTH_BITS, 24);
			break;
		}

		/* 32-bit */
		default:
		case 32:
		{
            glfwWindowHint(GLFW_RED_BITS, 8);
            glfwWindowHint(GLFW_GREEN_BITS, 8);
            glfwWindowHint(GLFW_BLUE_BITS, 8);
            glfwWindowHint(GLFW_ALPHA_BITS, 8);
            glfwWindowHint(GLFW_DEPTH_BITS, 32);
			break;
		}
	}

    p_window = glfwCreateWindow(width, height, "Cocos2dx-Linux", NULL, NULL);
    eResult = p_window ? true : false;

	/* Success? */
	if(eResult)
	{
        glfwMakeContextCurrent(p_window);

		/* Updates actual size */
        int iWidth, iHeight;
	  	glfwGetWindowSize(p_window, &iWidth, &iHeight);
		CCEGLViewProtocol::setFrameSize(iWidth, iHeight);		

		//set the init flag
		bIsInit = true;

		//register the glfw key event
		glfwSetKeyCallback(p_window, keyEventHandle);
		//register the glfw char event
		glfwSetCharCallback(p_window, charEventHandle);
		//register the glfw mouse event
		glfwSetMouseButtonCallback(p_window, mouseButtonEventHandle);
		//register the glfw mouse pos event
		glfwSetCursorPosCallback(p_window, cursorPosEventHandle);

		glfwSetWindowCloseCallback(p_window, closeEventHandle);

		//Inits extensions
		eResult = initExtensions();

		if (!eResult) {
			CCAssert(0, "fail to init the extensions of opengl");
		}
		initGL();
	}
}

void CCEGLView::setFrameZoomFactor(float fZoomFactor)
{
    m_fFrameZoomFactor = fZoomFactor;
    glfwSetWindowSize(p_window, m_obScreenSize.width * fZoomFactor, m_obScreenSize.height * fZoomFactor);
    CCDirector::sharedDirector()->setProjection(CCDirector::sharedDirector()->getProjection());
}

float CCEGLView::getFrameZoomFactor()
{
    return m_fFrameZoomFactor;
}

void CCEGLView::setViewPortInPoints(float x , float y , float w , float h)
{
    glViewport((GLint)(x * m_fScaleX * m_fFrameZoomFactor+ m_obViewPortRect.origin.x * m_fFrameZoomFactor),
        (GLint)(y * m_fScaleY * m_fFrameZoomFactor + m_obViewPortRect.origin.y * m_fFrameZoomFactor),
        (GLsizei)(w * m_fScaleX * m_fFrameZoomFactor),
        (GLsizei)(h * m_fScaleY * m_fFrameZoomFactor));
}

void CCEGLView::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint)(x * m_fScaleX * m_fFrameZoomFactor + m_obViewPortRect.origin.x * m_fFrameZoomFactor),
              (GLint)(y * m_fScaleY * m_fFrameZoomFactor + m_obViewPortRect.origin.y * m_fFrameZoomFactor),
              (GLsizei)(w * m_fScaleX * m_fFrameZoomFactor),
              (GLsizei)(h * m_fScaleY * m_fFrameZoomFactor));
}


bool CCEGLView::isOpenGLReady()
{
	return bIsInit;
}

void CCEGLView::end()
{
	/* Exits from GLFW */
	glfwTerminate();
	delete this;
	exit(0);
}

void CCEGLView::swapBuffers() {
	if (bIsInit) {
		/* Swap buffers */
		glfwSwapBuffers(p_window);
        glfwPollEvents();
	}
}

void CCEGLView::setIMEKeyboardState(bool bOpen) {

}

bool CCEGLView::initGL()
{
    GLenum GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult) 
    {
        fprintf(stderr,"ERROR: %s\n",glewGetErrorString(GlewInitResult));
        return false;
    }

    if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
    {
        CCLog("Ready for GLSL");
    }
    else 
    {
        CCLog("Not totally ready :(");
    }

    if (glewIsSupported("GL_VERSION_2_0"))
    {
        CCLog("Ready for OpenGL 2.0");
    }
    else
    {
        CCLog("OpenGL 2.0 not supported");
    }

    // Enable point size by default on linux.
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    return true;
}

void CCEGLView::destroyGL()
{
	/*
    if (m_hDC != NULL && m_hRC != NULL)
    {
        // deselect rendering context and delete it
        wglMakeCurrent(m_hDC, NULL);
        wglDeleteContext(m_hRC);
    }
	*/
}

CCEGLView* CCEGLView::sharedOpenGLView()
{
    static CCEGLView* s_pEglView = NULL;
    if (s_pEglView == NULL)
    {
        s_pEglView = new CCEGLView();
    }
    return s_pEglView;
}

NS_CC_END
