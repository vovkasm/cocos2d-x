#!/usr/bin/python

import os, os.path
import platform
import subprocess
import sys

cfg = dict()
cfg['Ubuntu 14.04'] = dict()
cfg['Ubuntu 14.04']['repos'] = 'ppa:ubuntu-toolchain-r/test'
cfg['Ubuntu 14.04']['packages'] = '''
    libx11-dev
    libxmu-dev
    libglu1-mesa-dev
    libgl2ps-dev
    libxi-dev
    g++-4.9
    libzip-dev
    libpng12-dev
    libcurl4-gnutls-dev
    libfontconfig1-dev
    libsqlite3-dev
    libglew-dev
    libssl-dev
'''

def get_config_for_keys(key, distname, distversion):
    ret = []
    root_keys = filter(lambda x: x in cfg, [distname, '%s %s' % (distname, distversion)])
    for root_key in root_keys:    
        if key in cfg[root_key]:
            ret.extend(cfg[root_key][key].split())

    return ret

def add_apt_repository(repo):
    skip = False
    if repo.startswith('ppa:'):
        repo_name = repo.partition(':')[2]

    output = subprocess.check_output(['apt-cache', 'policy'])
    if repo_name in output:
        skip = True

    if not skip:
        print "Add %s to repositories..." % (repo)
        subprocess.check_call(['sudo', 'add-apt-repository', '-y', repo])

def if_apt_package_missing(pkg):
    output = subprocess.check_output(['dpkg-query', '-W', "--showformat='${Status}'", pkg])
    return "install ok installed" not in output

def linux_main():
    distname, distversion, distid = platform.linux_distribution()
    print "Run on: %s %s (%s)" % (distname, distversion, distid)

    script_dir = os.path.dirname(os.path.realpath(__file__))
    cocos_dir = os.path.dirname(script_dir)
    print "cocos dir: %s" % (cocos_dir)

    repos = get_config_for_keys('repos', distname, distversion)
    for repo in repos:
        add_apt_repository(repo)

    print "Update packages list..."
    subprocess.check_call(['sudo', 'apt-get', 'update', '-q', '-y'])

    print "Check missing dependencies..."
    required_packages = get_config_for_keys('packages', distname, distversion)
    missing_packages = filter(if_apt_package_missing, required_packages)

    if len(missing_packages) > 0:
        print "Install missing packages: %s" % (missing_packages)
        cmd = ['sudo', 'apt-get', 'install', '-q', '-y']
        cmd.extend(missing_packages)
        subprocess.check_call(cmd)

    print "Done."

if __name__ == '__main__':
    
    if not sys.platform.startswith('linux'):
        raise Exception("This script only for linux")

    linux_main()
