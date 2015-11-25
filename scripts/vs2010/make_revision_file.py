# -*- coding: windows-1251 -*-
import subprocess, sys, os, re

VersionFile = '../version/qnetmap_revision.h'
VersionDir = '..'

Switches = '-n'
if len(sys.argv) > 3:
	Switches = Switches + sys.argv[3] 

Process = subprocess.Popen(['svnversion', Switches, VersionDir], stdout=subprocess.PIPE)
Version = Process.stdout.read()

# В конце может быть буква
Pattern = re.compile(r'\d+:(?P<Second>\d+)')
Match = Pattern.search(Version)
if Match:
	Version = Match.group('Second')
else:
	Pattern = re.compile('\d+')
	Match = Pattern.search(Version)
	if Match:
		Version = Match.group()
	else:
		exit(1)

try:
	File = open(VersionFile, 'r')
except IOError:
	Contents = ''
else:
	Contents = File.read()
	File.close()

if Contents == Version:
	exit(0)

if Contents != '':
	os.unlink(VersionFile)

File = open(VersionFile, 'w')
File.write(Version)
File.close()
