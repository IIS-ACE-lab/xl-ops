# Reproducible artifact generation for the XL cost paper.
#
# Main target:
#   make paper-artifacts
#
# This builds XL-test, generates prediction data, get_D lookup files,
# LaTeX tables, LaTeX formula snippets, figure snippets, and provenance
# metadata under:
#
#   paper-artifacts/

SAGE := sage
PYTHON := python3

ART := paper-artifacts
DATA := data/processed

XLTEST := src/bin/XL-test

N_MIN := 10
N_MAX := 400
M_FACTOR := 2

PRED_CSV := $(DATA)/predictions-allq-m2n-n010-400.csv
GET_D_FILES := \
	$(DATA)/get_D-2.txt \
	$(DATA)/get_D-31.txt \
	$(DATA)/get_D-256.txt

ERROR_DATA := data/raw/prediction-errors

.PHONY: all build data tables formulas figures split-predictions prediction-error-artifacts metadata paper-artifacts clean-artifacts clean-data

all: paper-artifacts

# --------------------------------------------------------------------
# Build C implementation
# --------------------------------------------------------------------

build:
	$(MAKE) -C src

# --------------------------------------------------------------------
# Prediction CSV
# --------------------------------------------------------------------

$(PRED_CSV): build sage/xl_predict_range.sage sage/xl_cost_compare.sage sage/xl_cost_formulas.sage
	mkdir -p $(DATA)
	$(SAGE) sage/xl_predict_range.sage \
		--compare-file sage/xl_cost_compare.sage \
		--exe $(XLTEST) \
		--n-min $(N_MIN) \
		--n-max $(N_MAX) \
		--m-factor $(M_FACTOR) \
		--output $@

# --------------------------------------------------------------------
# get_D lookup files required by security_levels_csv.py
# --------------------------------------------------------------------

$(DATA)/get_D-%.txt: sage/get_D.sage
	mkdir -p $(DATA)
	q=$*; \
	{ \
	  echo "import sys"; \
	  for i in $$(seq $(N_MIN) $(N_MAX)); do \
	    m=$$(($(M_FACTOR) * i)); \
	    printf "sys.argv = ['get_D.sage', '-q', '%s', '-n', '%s', '-m', '%s', '-s']\n" "$$q" "$$i" "$$m"; \
	    echo 'load("sage/get_D.sage")'; \
	  done; \
	} | $(SAGE) | tail -n +5 | sed 's/^\(sage: \)\+//g' > $@

data: $(PRED_CSV) $(GET_D_FILES)

# --------------------------------------------------------------------
# Paper tables
# --------------------------------------------------------------------

tables: data scripts/fukuoka_table.py scripts/security_levels_csv.py sage/get_D.sage sage/generate_guessing_table.sage sage/xl_cost_compare.sage sage/xl_cost_formulas.sage
	mkdir -p $(ART)/tables $(ART)/data
	cp $(PRED_CSV) $(ART)/data/predictions-allq-m2n-n010-400.csv
	cp $(GET_D_FILES) $(ART)/data/
	$(PYTHON) scripts/fukuoka_table.py \
		--get-d sage/get_D.sage \
		--xl-test "$(SAGE) sage/xl_cost_compare.sage --exe $(XLTEST)" \
		> $(ART)/tables/fukuoka_table.tex
	$(PYTHON) scripts/security_levels_csv.py \
		--csv $(PRED_CSV) \
		--get-d-dir $(DATA) \
		--latex \
		--show-cost \
		> $(ART)/tables/security_levels.tex
	$(SAGE) sage/generate_guessing_table.sage \
		--compare-file sage/xl_cost_compare.sage \
		--exe $(XLTEST) \
		--output $(ART)/tables/guessing_table.tex


prediction-error-artifacts: scripts/analyze_prediction_errors.py
	test -d $(ERROR_DATA) || { echo "Missing $(ERROR_DATA). Generate or copy prediction-error logs first."; exit 1; }
	mkdir -p $(ART)/tables $(ART)/figures
	$(PYTHON) scripts/analyze_prediction_errors.py \
		--input-dir $(ERROR_DATA) \
		--table-output $(ART)/tables/prediction_error_table.tex \
		--figure-output $(ART)/figures/prediction_error_plots.tex \
		--quiet

# --------------------------------------------------------------------
# LaTeX formula snippets
# --------------------------------------------------------------------

formulas: build sage/xl_latex_formulas.sage sage/xl_cost_formulas.sage
	mkdir -p $(ART)/formulas
	$(SAGE) sage/xl_latex_formulas.sage \
		--table-I --style single-fraction \
		> $(ART)/formulas/table-I.tex
	$(SAGE) sage/xl_latex_formulas.sage \
		--table-II --style sage \
		> $(ART)/formulas/table-II.tex
	$(SAGE) sage/xl_latex_formulas.sage \
		--all-base --style sage \
		> $(ART)/formulas/op-formulas-base.tex
	$(SAGE) sage/xl_latex_formulas.sage \
		--all-const --style sage \
		> $(ART)/formulas/op-formulas-const.tex
	$(SAGE) sage/xl_latex_formulas.sage \
		--all-const-bucket --style sage \
		> $(ART)/formulas/op-formulas-const-bucket.tex
	$(SAGE) sage/xl_latex_formulas.sage \
		--extra-BM --style factor \
		> $(ART)/formulas/bm-extra.tex
	$(SAGE) sage/xl_latex_formulas.sage \
		--bit-costs --xl-test $(XLTEST) --style sage --inline-extra-BM \
		> $(ART)/formulas/all-bit-ops.tex
	$(SAGE) sage/xl_latex_formulas.sage \
		--op-cost \
		--xl-test $(XLTEST) \
		> $(ART)/formulas/op-costs.tex

# --------------------------------------------------------------------
# Figure snippets
# --------------------------------------------------------------------

SPLIT_PRED_DIR := $(ART)/figures/data

split-predictions: data scripts/split_predictions.sh
	mkdir -p $(SPLIT_PRED_DIR)
	bash scripts/split_predictions.sh $(PRED_CSV) $(SPLIT_PRED_DIR)

figures: data split-predictions scripts/quotient_convergence_grid_csv.py
	mkdir -p $(ART)/figures
	$(PYTHON) scripts/quotient_convergence_grid_csv.py \
		--csv $(PRED_CSV) \
		--get-d-dir $(DATA) \
		--output $(ART)/figures/quotient_convergence_grid.tex

# --------------------------------------------------------------------
# Provenance metadata
# --------------------------------------------------------------------

metadata:
	mkdir -p $(ART)
	git rev-parse HEAD > $(ART)/CODE_VERSION
	git status --short > $(ART)/CODE_STATUS

# --------------------------------------------------------------------
# Top-level artifact target
# --------------------------------------------------------------------

paper-artifacts: tables formulas figures metadata

# --------------------------------------------------------------------
# Cleaning
# --------------------------------------------------------------------

clean-artifacts:
	rm -rf $(ART)

clean-data:
	rm -rf $(DATA)

