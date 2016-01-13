#!/usr/bin/python

import sys, re, string

fname = ''
for n in range(len(sys.argv)):
    if (n > 0):
        fname = str(sys.argv[n]);
if (fname == ''):
    print 'no input file'
    sys.exit()

reps = [
    [ 'Acta Phys. Polon.B' , '\\\\appolb{' ],
    [ 'Acta Phys. Polon.D' , '\\\\appold{' ],
    [ 'Adv. Ser. Direct. High Energy Phys.' , '\\\\asdhep{' ],
    [ 'Ann. Phys.' , '\\\\ap{' ],
    [ 'Ann. Rev. Nucl. Part. Sci.' , '\\\\arnps{' ],
    [ 'Commun. Math. Phys.' , '\\\\cmp{' ],
    [ 'Comp. Phys. Commun.' , '\\\\cpc{' ],
    [ 'Comput. Phys. Commun.' , '\\\\cpc{' ],
    [ 'Eur. Phys. J.B' , '\\\\epjb{' ],
    [ 'Eur. Phys. J.C' , '\\\\epjc{' ],
    [ 'Eur. Phys. J.' , '\\\\epj{' ],
    [ 'Int. J. Mod. Phys.A' , '\\\\ijmpa{' ],
    [ 'Int. J. Mod. Phys.' , '\\\\ijmp{' ],
    [ 'J. Grid Comput.C' , '\\\\jgcc{' ],
    [ 'J. High Energy Phys.' , '\\\\jhep{' ],
    [ 'JHEP' , '\\\\jhep{' ],
    [ 'J. Phys.C' , '\\\\jphc{' ],
    [ 'J. Phys.G' , '\\\\jphg{' ],
    [ 'J. Phys.' , '\\\\jph{' ],
    [ 'Nucl. Instrum. Meth.' , '\\\\nim{' ],
    [ 'Nucl. Phys.A' , '\\\\npa{' ],
    [ 'Nucl. Phys.B' , '\\\\npb{' ],
    [ 'Nucl. Phys.G' , '\\\\npg{' ],
    [ 'Nucl. Phys. (Proc. Suppl.)' , '\\\\npps{' ],
    [ 'Nucl. Phys.' , '\\\\np{' ],
    [ 'Nuovo Cim.' , '\\\\nc{' ],
    [ 'Phys. Atom. Nucl.' , '\\\\pan{' ],
    [ 'Phys. Lett.A' , '\\\\pla{' ],
    [ 'Phys. Lett.B' , '\\\\plb{' ],
    [ 'Phys. Lett.' , '\\\\pl{' ],
    [ 'Phys. Rev.B' , '\\\\prb{' ],
    [ 'Phys. Rev.C' , '\\\\prc{' ],
    [ 'Phys. Rev.D' , '\\\\prd{' ],
    [ 'Phys. Rev. Lett.' , '\\\\prl{' ],
    [ 'Phys. Rev.' , '\\\\pr{' ],
    [ 'Phys. Rept.' , '\\\\prep{' ],
    [ 'Phys. ScriptaB' , '\\\\psab{' ],
    [ 'Physica' , '\\\\phys{' ],
    [ 'PoS' , '\\\\pos{' ],
    [ 'Rev. Mod. Phys.' , '\\\\rmp{' ],
    [ 'Sov. J. Nucl. Phys.' , '\\\\sjnp{' ],
    [ 'Sov. Phys. JETP' , '\\\\jetp{' ],
    [ 'Sov. Phys. JETP Lett.' , '\\\\jetpl{' ],
    [ 'JETP Lett.' , '\\\\jetpl{' ],
    [ 'Yad. Fiz.' , '\\\\yf{' ],
    [ 'Z. Phys.B' , '\\\\zpb{' ],
    [ 'Z. Phys.C' , '\\\\zpc{' ],
    [ 'Z. Phys.' , '\\\\zp{' ],
    [ 'Zh. Eksp. Teor. Fiz.' , '\\\\zetf{' ]
    ]

tnoc = open('jhep_'+fname,'w')
cont = open(fname,'r').readlines()

refs = [
    re.compile('journal\s=\s{(?P<ref>[^}]*)}.*,'),
    re.compile('volume\s=\s{(?P<ref>[^}]*)}.*,'),
    re.compile('year\s=\s{(?P<ref>[^}]*)}.*,'),
    re.compile('pages\s=\s{(?P<ref>[^}]*)}.*,')
    ]
vals = [ "" , "" , "" , "" ]
lines = [ "" , "" , "" , "" ]

entry = 0
cache = [ ]

eref = re.compile('@(article|book){(?P<eref>.*),',re.IGNORECASE)
fere = re.compile('^\s*}\s*$')

print '\nprocessing file \''+fname+'\'\n'
for line in cont:
    ms = eref.search(line)
    if (ms):
        print 'Entry \''+ms.group('eref')+'\' {'
        for n in range(0,4):
	    lines[n] = ""
	    vals[n] = ""
	cache = [ ]
	entry = 1
    if (entry == 0):
	tnoc.write(line)
	continue
    cache.append(line)
    for n in range(len(refs)):
        ms = refs[n].search(line)
        if (ms):
            vals[n] = ms.group('ref')
	    lines[n] = line
	    if (n == 0): jpos = len(cache)-1
	    break
    ms = fere.search(line)
    if (ms):
	jhep = 'journal = {'+vals[0]+vals[1]\
	    +'}{'+vals[2]+'}{'+vals[3]+'}},'
	rep = 0
	for n in range(len(reps)):
	    phej = re.sub(reps[n][0],reps[n][1],jhep)
	    if (phej != jhep):
		jhep = phej
		rep = 1
		break
	if (rep == 1):
	    cache[jpos] = '  '+jhep+'\n'
	    print cache[jpos],
	    for n in range(1,4): 
	        if (lines[n]!=""):
		    cache.remove(lines[n])
        for n in range(len(cache)):
            tnoc.write(cache[n])
	entry = 0
        print '}'

print '\noutput written to \'jhep_'+fname+'\'\n'
