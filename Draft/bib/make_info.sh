#!/usr/bin/python

import sys, re, string

fname = ''
for n in range(len(sys.argv)):
    if (n > 0):
        fname = str(sys.argv[n]);
if (fname == ''):
    print 'no input file'
    sys.exit()

tnoc = open('info_'+fname,'w')
cont = open(fname,'r').readlines()

nkey = re.compile("(?P<ref>[A-Z]..)")
ykey = re.compile("[0-9]{2}(?P<ref>[0-9]{2})")
refs = [
    re.compile('author\s=\s{(?P<ref>.*)},',re.DOTALL),
    re.compile('title\s=\s{(?P<ref>.*)},',re.DOTALL),
    re.compile('journal\s=\s{(?P<ref>[^}]*)}.*,'),
    re.compile('volume\s=\s{(?P<ref>[^}]*)}.*,'),
    re.compile('year\s=\s{(?P<ref>[^}]*)}.*,'),
    re.compile('pages\s=\s{(?P<ref>[^}]*)}.*,'),
    re.compile('eprint\s=\s{(?P<ref>[^}]*)}.*,')
    ]
vals = [ "" , "" , "" , "" , "" , "" , "" ]

entry = 0
cache = ref = key = ""
kmap  = []
emap  = {}

eref = re.compile('@(article|book){(?P<eref>.*),',re.IGNORECASE)
fere = re.compile('^\s*}\s*$',re.MULTILINE)

print '\nprocessing file \''+fname+'\'\n'
for line in cont:
    line = cache+line
    ms = eref.search(line)
    if (ms):
	ref = ms.group('eref')
        print 'Entry \''+ref+'\' {'
        for n in range(len(refs)):
	    vals[n] = ""
	cache = ""
	entry = 1
	continue
    if (entry == 0):
	continue
    cache = line
    for n in range(len(refs)):
        ms = refs[n].search(line)
        if (ms):
            vals[n] = ms.group('ref')
	    cache = ""
	    break
    ms = fere.search(line)
    if (ms):
        ref = re.sub(":","",ref)
        key = ""
	ms = nkey.search(ref)
	if (ms):
	    key += ms.group('ref')
	ms = ykey.search(ref)
	if (ms):
	    key += ms.group('ref')
	ckh = 96
	ckey = key
	while (ckey in kmap):
	    ckh += 1
	    ckey = key+chr(ckh)
	kmap.append(ckey)
	vals[0] = re.sub("\n"," ",vals[0])
        vals[0] = re.sub("~"," ",vals[0])
        vals[0] = re.sub("\\\\\\\"","",vals[0])
        vals[0] = re.sub("{","",vals[0])
        vals[0] = re.sub("}","",vals[0])
	auths = re.split("\sand\s",vals[0])
	akey = re.compile("(?P<last>.*),(?P<first>.*)")
	xkey = re.compile("(?P<first>.*)\s(?P<last>[A-Za-z]*)")
	fkey = re.compile("(?P<init>[A-Z])[a-z]*[^\.]")
	vals[0]= ""
	acnt = 0
	for a in auths:
	    acnt += 1
	    if (acnt>1):
		vals[0] += ' and '
	    ms = akey.search(a)
	    if (ms):
		fn = ms.group('first')
	    else:
		ms = xkey.search(a)
		if (ms):
		    fn = ms.group('first')
	    if (ms):
		fn = fkey.sub(r'\g<init>. ',fn)
		vals[0] += fn+' '+ms.group('last')
	    else:
		vals[0] += a
	for n in range(len(refs)):
	    vals[n] = re.sub(","," ",vals[n])
	ovl = ref+','+ckey+','+vals[0]+','+vals[1];
	if (vals[2]!=""):
	    ovl += ','+vals[2]+','+vals[3]+',('+vals[4]+'),'+vals[5]+',';
	else:
	    ovl += ',,,,,'
	if (vals[6]!=""):
	    if (re.match("hep",vals[6])==None):
		vals[6] = 'arXiv:'+vals[6]
	    if (vals[2]!=""):
		ovl += '['+vals[6]+'],'+vals[6]
	    else:
		ovl += vals[6]+','+vals[6]
	else:
	    ovl+=','
	    if (vals[2]!=""):
		ovl += 'F+J+'+vals[2]+'%2C'+vals[3]\
		    +'%2C'+re.sub(r'-.*','',vals[5]);
        ovl = re.sub("\n"," ",ovl)
        ovl = re.sub("~"," ",ovl)
        ovl = re.sub("\\\\\\\"","",ovl)
        ovl = re.sub("\\\[.]","",ovl)
        ovl = re.sub("{","",ovl)
        ovl = re.sub("}","",ovl)
        ovl = re.sub("\$","",ovl)
        ovl = re.sub("\\\\","",ovl)
        ovl = re.sub("\t"," ",ovl)
        ovl = re.sub("  "," ",ovl)
	entry = 0
	emap[ckey] = ovl
        print "  "+ckey+" -> '"+ovl+"'"
        print '}'
keys = emap.keys()
keys.sort()
for key in keys:
    tnoc.write('@mybibitem{'+emap[key]+'}\n')

print '\noutput written to \'info_'+fname+'\'\n'
