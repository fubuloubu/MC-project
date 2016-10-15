# LaTeX makefile for documentation
# Note: run it twice to ensure LastPage works
.PHONY: %.pdf
%.pdf: %.tex
	@echo " LATEX $<"
	@pdflatex -shell-escape -interaction=batchmode $< > /dev/null || \
		(echo && echo "Error:" && echo && cat $*.log | grep -A 10 ^! && rm $@ && exit 1)
	@pdflatex -shell-escape -interaction=batchmode $< > /dev/null
	@if [ -d "$$(readlink -f ~/Downloads)" ]; then \
		echo "  MOVE $@"; \
		mv $@ ~/Downloads; \
	fi;

# If we have other specific rules, include them here
-include *.mk

# Status rule to check how much we have left
# Uses TODO and DONE flags in files
.PHONY: status
status:
	@echo "STATUS $${PWD##*/}/*.*"
	@\
for file in `ls *.* | egrep -v "(template)"`; do \
	todo_togo=$$(grep -c TODO $$file); \
	todo_done=$$(grep -c DONE $$file); \
	total=$$(expr $$todo_done + $$todo_togo); \
	if [ $$total -ne 0 ]; then \
		percent=$$(expr 100 \* $$todo_done / $$total); \
		echo "$$file: $$percent% complete ($$todo_done DONE of $$total TODO items)"; \
	fi \
done;

# Clean rule to remove intermediates produced by LaTeX and relevant libraries
.PHONY: clean
clean:
	@echo " CLEAN documentation"
	@rm -f *.pdf*
	@rm -f *.log
	@rm -f *.out
	@rm -f *.aux
	@rm -f *.pyg
	@rm -rf _minted-*

# Initialize repo
.PHONY: init
init:
	# Setup target directory
	@-mkdir -p ~/Downloads;
	# Setup git (and latex)
	@\
echo "Enter your full name and press [ENTER]:" && \
read name && \
git config user.name "$$name" && \
echo "Enter your uni and press [ENTER]:" && \
read uni && \
git config user.uni "$$uni" && \
echo "Setting email to $$uni@columbia.edu..." && \
git config user.email "$$uni@columbia.edu";
