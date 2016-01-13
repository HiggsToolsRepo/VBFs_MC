
PROTECT = .bib .bst .sty .svn feynmp Makefile

SHELL = /bin/bash

FORMAT = $(shell ttp=$$(grep -e '\\pdfoutput' $(MASTER).tex | \
  awk '{ split($$1,a,"="); print a[2]; }'); if [ "$$ttp" == "1" ]; \
    then echo "pdf"; else echo "ps"; fi)

DEFFIG = $(shell ttp=$$(grep -e '\\pdfoutput' $(MASTER).tex | \
  awk '{ split($$1,a,"="); print a[2]; }'); if [ "$$ttp" == "1" ]; \
    then echo "pdf"; else echo "eps"; fi)

NODEPS = $(shell andeps=$$(find .); \
  for j in $(PROTECT); do nndeps=$$(echo $$andeps | \
    sed -e's/ [^\t ]*'$$(echo $$j | sed -e's/\//\\\//g')'[^\t ]*//g'); \
    andeps=$$nndeps; done; \
  for j in $(DEPFILES); do nndeps=$$(echo $$andeps | \
    sed -e's/[^\t ]*'$$(echo $$j | sed -e's/\//\\\//g')' //g'); \
    andeps=$$nndeps; \
  done; echo $$andeps;)
DEPS = $(shell fsdeps()\
{\
  if $$(echo $$1 | awk '{ exit match($$1,".tex"); }') ||\
     ! test -f $$1; then return; fi;\
  depls=$$(grep -e'\\bibliography{' $$1);\
  for i in $$depls; do\
    sdep=$$(echo $$i | awk '{ \
	split($$1,l,"%"); \
	if (split(l[1],a,"bibliography{")<2) exit; \
        split(a[2],b,"}"); \
	split(b[1],c,","); \
	for (i=1; i<=length(c); i++) { \
	  if (match(c[i],"[.]bib")==0) c[i]=c[i]".bib"; \
	  print c[i]; \
	} \
        }');\
    for bibdep in $$sdep; do if test -f $$bibdep; then echo $$bibdep; fi; done;\
  done;\
  depls=$$(grep -e'\\input' $$1);\
  for i in $$depls; do\
    sdep=$$(echo $$i | awk '{ split($$1,l,"%"); \
	if (split(l[1],a,"\\\\input{")<2) exit; split(a[2],b,"}"); \
	if (match(b[1],"[.]tex")==0) b[1]=b[1]".tex"; print b[1]; }');\
    if test -f $$sdep; then echo $$sdep; fi\
  done;\
  depls=$$(grep -e'\\includegraphics' $$1);\
  for i in $$depls; do\
    sdep=$$(echo $$i | awk '{ split($$1,l,"%"); \
	if (split(l[1],a,"\\\\includegraphics")<2) exit; \
	split(a[2],b,"{"); \
	split (b[2],c,"}"); \
        if (match(c[1],"[.]pdf")==0 && match(c[1],"[.]png")==0 && \
          match(c[1],"[.]jpg")==0 && match(c[1],"[.]eps")==0 && \
          match(c[1],"[.]ps")==0) c[1]=c[1]".$(DEFFIG)"; \
	print c[1]; }');\
    if test -f $$sdep; then echo $$sdep; fi\
  done;\
};\
fadeps()\
{\
  for i in $$*; do\
    let ++cnt;\
    adeps[$$cnt]=$$(fsdeps $$i);\
    fadeps $${adeps[$$cnt]};\
  done\
};\
let cnt=0; declare -a ais adeps;\
fadeps $(MASTER).tex;\
let i=0;\
while test $$i -lt $$cnt; do\
  let ++i;\
  for j in $${adeps[$$i]}; do \
    deps=$$deps" "$$j; \
  done;\
done;\
echo $$deps;)

FIGUREDEPS = $(shell figuredeps()\
{\
  if ! $$(echo $$1 | grep -q \.tex) || ! test -f $$1; then return; fi;\
  depls=$$(grep -e'\\includegraphics' $$1);\
  for i in $$depls; do\
    sdep=$$(echo $$i | awk '{ split($$1,l,"%"); \
	if (split(l[1],a,"\\\\includegraphics")<2) exit; \
	split(a[2],b,"{"); \
	split (b[2],c,"}"); \
        if (match(c[1],"[.]pdf")==0 && match(c[1],"[.]png")==0 && \
          match(c[1],"[.]jpg")==0 && match(c[1],"[.]eps")==0 && \
          match(c[1],"[.]ps")==0) c[1]=c[1]".$(DEFFIG)"; \
	print c[1]; }');\
    echo $$sdep; \
  done;\
};\
for i in $(MASTER).tex $(DEPS); do \
  deps=$$deps" "$$(figuredeps $$i); \
done; \
echo $$deps;)

DEPFILE = $(MASTER).tdeps

include $(DEPFILE)

%.tdeps: $(DEPFILES)
	@echo -e "DEPFILES = $(MASTER).tex $(DEPS)" > $(DEPFILE)
	@echo -e "FIGUREDEPFILES = $(FIGUREDEPS)" >> $(DEPFILE)

%.aux: %.tdeps
	$(MAKE) $(addsuffix .figures,$(basename $@)); \
	latex -interaction=nonstopmode $(addsuffix .tex,$(basename $@))

%.bbl: %.tdeps
	if egrep -s 'bibliography' $(addsuffix .tex,$(basename $@)); \
	  then bibtex $(basename $@); fi

%.graphs: $(DEPFILES)
	@g=$$(egrep {fmffile} $(filter %.tex,$(DEPFILES)) | \
	      egrep begin | cut -d{ -f3 | cut -d} -f1); \
	t=$$(egrep feyn $(filter %.tex,$(DEPFILES)) | \
	      egrep package | cut -d{ -f2 | cut -d} -f1); \
	if [ "$$t" = "feynmp" ]; then \
	  if test -f $$g.mp; then mpost $$g.mp; fi; \
	elif [ "$$t" = "feynmf" ]; then \
	  if test -f $$g.mf; then mf $$g.mf; fi; fi; 

%.figures: $(FIGUREDEPFILES)
	@echo "Recreated figures.";

figures/%.conf:
	@echo "No figures/*.conf found."

figures/%.dat:
	@echo "Did not find $@. Not running make-plots."

figures/%.pdf: figures/%.dat figures/make-plots.conf
	@datfile=$(addsuffix .dat,$(basename $@)); \
	if test -f $$datfile; then \
	if test -f figures/make-plots.conf; then addarg="-c figures/make-plots.conf"; fi; \
	bib/make-plots $$addarg --full-range $$datfile; \
	fi;

figures/%.eps: figures/%.dat figures/make-plots.conf
	@datfile=$(addsuffix .dat,$(basename $@)); \
	if test -f $$datfile; then \
	if test -f figures/make-plots.conf; then addarg="-c figures/make-plots.conf"; fi; \
	bib/make-plots --eps $$addarg --full-range $$datfile; \
	fi;

.SECONDARY:

%.ps: %.tex %.aux
	$(MAKE) $(addsuffix .bbl,$(basename $@)); \
	$(MAKE) $(addsuffix .graphs,$(basename $@)); \
	$(MAKE) $(addsuffix .figures,$(basename $@)); \
	lc=4; while ( egrep -s 'cross-references' \
	  $(addsuffix .log,$(basename $@)) || \
	  egrep -s 'reprocess' $(addsuffix .log,$(basename $@)) || \
	  egrep -s 'undefined' $(addsuffix .log,$(basename $@)) ) \
	  && [ $$lc -gt 0 ] ; do \
	    latex -interaction=nonstopmode $<; lc=`expr $$lc - 1`; done; \
	if test -f $(addsuffix .dvi,$(basename $@)); then \
	  dvips $(addsuffix .dvi,$(basename $@)); fi;

%.pdf: %.ps
	if test -f $(addsuffix .ps,$(basename $@)); then \
	  ps2pdf -sPAPERSIZE=a4 $(addsuffix .ps,$(basename $@)); fi;

$(MASTER): $(MASTER).$(FORMAT)

clean:
	rm -f $(MASTER).aux $(MASTER).log $(MASTER).dvi \
	  $(MASTER).bbl $(MASTER).bl $(MASTER).tdeps $(MASTER).pdf \
	  $(MASTER).ps $(MASTER).blg $(MASTER).out $(MASTER).toc

figures-clean:
	for i in $(filter figures/%.pdf,$(FIGUREDEPFILES)); do \
	datfile=$$(dirname $$i)/$$(basename $$i .pdf).dat; \
	if test -f $$datfile; then rm -f $$i; fi; \
	done;

dist: 
	mkdir $(MASTER); \
	for i in *; do if [ "$$i" != "$(MASTER)" ]; then \
	  cp -r $$i $(MASTER); fi; done; cd $(MASTER); \
	rm -f $(NODEPS) $$(find . -name \*~); cd -; \
	tar -czvf $(MASTER).tar.gz $(MASTER); rm -rf $(MASTER)

find-unneeded:
	@for i in $$(find * | grep -v "\.svn" | grep -v "\.cls" | grep -v "\.clo" | grep -v "\.sty" | grep -v "\.bbl"); do \
	  if test -f "$$i" -a "x$$(grep -F $$i $(MASTER).tdeps)" = "x"; then \
            echo $$i; \
          fi; \
        done

../$(MASTER)-submit:
	@lwd=$$PWD; cd ..; \
	if test -e $(MASTER)-submit; then \
          echo "Error: $(MASTER)-submit exists." \
	  exit 1; \
	fi; \
	cp -rpL $$lwd $(MASTER)-submit; \
        cd $(MASTER)-submit; \
	make clean; \
	make; \
	unneeded=$$(make --quiet find-unneeded); \
	cp $(MASTER).bbl temp.bbl; \
	make clean; \
	mv temp.bbl $(MASTER).bbl; \
	rm -f Makefile; \
	echo "Removing " $$unneeded; \
	for i in $$unneeded; do rm -f $$i; done; \
	rm -rf bib/; \
	cp ../arXiv/bib/*.bst .; \
	for i in *.bst; do \
	  if ! grep -q $$(basename $$i .bst) $(MASTER).tex; \
          then rm $$i; fi; \
	done; \
	sed -e "s/\.\.\/bib\///g" -i $(MASTER).tex; \
	rm -rf $$(find . -name "\.svn"); \
	cd ..; \
	tar czf $(MASTER)-submit.tar.gz $(MASTER)-submit
