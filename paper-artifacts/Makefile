# Reproducible artifact generation for the XL cost paper.
#
# Main target:
#   make paper-artifacts
#
# Generated files are real Make targets. Aggregate targets such as
# data, tables, formulas, figures, and paper-artifacts have no recipes,
# so make only regenerates files whose prerequisites changed.

SAGE := sage
PYTHON := python3

ART := .
DATA := data

XLTEST := ../src/bin/XL_test

XLCOST := ../XL_cost.sage
XLCOSTFORMULAS := ../XL_cost_formulas.sage

N_MIN := 10
N_MAX := 400
M_FACTOR := 2

PRED_CSV := $(DATA)/predictions-allq-m2n-n010-400.csv
GET_D_FILES := \
	$(DATA)/get_D-2.txt \
	$(DATA)/get_D-31.txt \
	$(DATA)/get_D-256.txt

ERROR_DATA := prediction-errors
ERROR_DATA_STAMP := $(ERROR_DATA)/.generated
ERROR_TABLE := $(ART)/tables/prediction_error_table.tex
ERROR_FIGURE := $(ART)/figures/prediction_error_plots.tex

SPLIT_PRED_DIR := $(ART)/figures/data

TABLE_FILES := \
	$(ART)/tables/fukuoka_table.tex \
	$(ART)/tables/guessing_table.tex \
	$(ART)/tables/op-costs.tex

FORMULA_FILES := \
	$(ART)/formulas/table-I.tex \
	$(ART)/formulas/table-II.tex \
	$(ART)/formulas/op-formulas-base.tex \
	$(ART)/formulas/op-formulas-const.tex \
	$(ART)/formulas/op-formulas-const-bucket.tex \
	$(ART)/formulas/bm-extra.tex \
	$(ART)/formulas/all-bit-ops.tex

FIGURE_FILES := \
	$(ART)/figures/quotient_convergence_grid.tex

PAPER_DATA_FILES := \
	$(ART)/data/predictions-allq-m2n-n010-400.csv \
	$(ART)/data/get_D-2.txt \
	$(ART)/data/get_D-31.txt \
	$(ART)/data/get_D-256.txt

SPLIT_SENTINEL := $(SPLIT_PRED_DIR)/.split-done

METADATA_FILES := \
	$(ART)/CODE_VERSION \
	$(ART)/CODE_STATUS

.PHONY: all build all-data all-tables all-formulas all-figures split-predictions \
        prediction-error-artifacts metadata paper-artifacts \
        clean-artifacts clean-data clean-error-data

all: paper-artifacts

# --------------------------------------------------------------------
# Directory targets
# --------------------------------------------------------------------

#$(DATA):
#	mkdir -p $@
#
#$(ART):
#	mkdir -p $@

$(ART)/data:
	mkdir -p $@

$(ART)/tables:
	mkdir -p $@

$(ART)/formulas:
	mkdir -p $@

$(ART)/figures:
	mkdir -p $@

$(SPLIT_PRED_DIR):
	mkdir -p $@

# --------------------------------------------------------------------
# Build C implementation
# --------------------------------------------------------------------
#
# The binary target is real. If your src/Makefile has better dependency
# tracking, this still delegates to it. The wildcard prerequisites are enough
# to cause the top-level Makefile to rebuild when typical C sources change.

SRC_FILES := $(wildcard src/*.c src/*.h src/**/*.c src/**/*.h src/Makefile)

$(XLTEST): $(SRC_FILES)
	$(MAKE) -C ../src

build: $(XLTEST)

# --------------------------------------------------------------------
# Prediction CSV
# --------------------------------------------------------------------

$(PRED_CSV): $(XLTEST) predict_range.sage $(XLCOST) $(XLCOSTFORMULAS) | $(DATA)
	$(SAGE) predict_range.sage \
		--XL_cost $(XLCOST) \
		--exe $(XLTEST) \
		--n-min $(N_MIN) \
		--n-max $(N_MAX) \
		--m-factor $(M_FACTOR) \
		--output $@

# --------------------------------------------------------------------
# get_D lookup files
# --------------------------------------------------------------------

$(DATA)/get_D-%.txt: get_D.sage | $(DATA)
	q=$*; \
	{ \
	  echo "import sys"; \
	  for i in $$(seq $(N_MIN) $(N_MAX)); do \
	    m=$$(($(M_FACTOR) * i)); \
	    printf "sys.argv = ['get_D.sage', '-q', '%s', '-n', '%s', '-m', '%s', '-s']\n" "$$q" "$$i" "$$m"; \
	    echo 'load("get_D.sage")'; \
	  done; \
	} | $(SAGE) | tail -n +5 | sed 's/^\(sage: \)\+//g' > $@

all-data: $(PRED_CSV) $(GET_D_FILES)

# --------------------------------------------------------------------
# Copy processed data into paper-artifacts/data
# --------------------------------------------------------------------

#$(ART)/data/predictions-allq-m2n-n010-400.csv: $(PRED_CSV) | $(ART)/data
#	cp $< $@
#
#$(ART)/data/get_D-%.txt: $(DATA)/get_D-%.txt | $(ART)/data
#	cp $< $@

# --------------------------------------------------------------------
# Paper tables
# --------------------------------------------------------------------

$(ART)/tables/fukuoka_table.tex: generate_guessing_table.sage $(XLCOST) $(XLCOSTFORMULAS) $(XLTEST) | $(ART)/tables
	$(SAGE) generate_guessing_table.sage \
		--XL_cost $(XLCOST) \
		--exe $(XLTEST) \
		--guessing-output none \
		--fukuoka-output $@

$(ART)/tables/guessing_table.tex: generate_guessing_table.sage $(XLCOST) $(XLCOSTFORMULAS) $(XLTEST) | $(ART)/tables
	$(SAGE) generate_guessing_table.sage \
		--XL_cost $(XLCOST) \
		--exe $(XLTEST) \
		--guessing-output $@

$(ART)/tables/op-costs.tex: latex_formulas.sage $(XLCOSTFORMULAS) $(XLTEST) | $(ART)/tables
	$(SAGE) latex_formulas.sage \
		--formula-file $(XLCOSTFORMULAS) \
		--op-cost \
		--xl-test $(XLTEST) \
		> $@


all-tables: $(TABLE_FILES) $(PAPER_DATA_FILES)

# --------------------------------------------------------------------
# Optional prediction-error artifacts
# --------------------------------------------------------------------

$(ERROR_DATA_STAMP): gen_prediction_error_data.sh $(XLCOST) $(XLCOSTFORMULAS) $(XLTEST)
	mkdir -p $(ERROR_DATA)
	bash gen_prediction_error_data.sh \
		--out-dir $(ERROR_DATA) \
		--sage $(SAGE) \
		--XL_cost $(XLCOST) \
		--XL_test $(XLTEST) \
		--jobs 90% \
		--repeats 100

$(ERROR_TABLE) $(ERROR_FIGURE): analyze_prediction_errors.py $(ERROR_DATA_STAMP) | $(ART)/tables $(ART)/figures
	$(PYTHON) analyze_prediction_errors.py \
		--input-dir $(ERROR_DATA) \
		--table-output $(ERROR_TABLE) \
		--figure-output $(ERROR_FIGURE) \
		--quiet

prediction-error-artifacts: $(ERROR_TABLE) $(ERROR_FIGURE)

# --------------------------------------------------------------------
# LaTeX formula snippets
# --------------------------------------------------------------------

$(ART)/formulas/table-I.tex: latex_formulas.sage $(XLCOSTFORMULAS) | $(ART)/formulas
	$(SAGE) latex_formulas.sage \
		--formula-file $(XLCOSTFORMULAS) \
		--table-I --style single-fraction \
		> $@

$(ART)/formulas/table-II.tex: latex_formulas.sage $(XLCOSTFORMULAS) | $(ART)/formulas
	$(SAGE) latex_formulas.sage \
		--formula-file $(XLCOSTFORMULAS) \
		--table-II --style sage \
		> $@

$(ART)/formulas/op-formulas-base.tex: latex_formulas.sage $(XLCOSTFORMULAS) | $(ART)/formulas
	$(SAGE) latex_formulas.sage \
		--formula-file $(XLCOSTFORMULAS) \
		--all-base --style sage \
		> $@

$(ART)/formulas/op-formulas-const.tex: latex_formulas.sage $(XLCOSTFORMULAS) | $(ART)/formulas
	$(SAGE) latex_formulas.sage \
		--formula-file $(XLCOSTFORMULAS) \
		--all-const --style sage \
		> $@

$(ART)/formulas/op-formulas-const-bucket.tex: latex_formulas.sage $(XLCOSTFORMULAS) | $(ART)/formulas
	$(SAGE) latex_formulas.sage \
		--formula-file $(XLCOSTFORMULAS) \
		--all-const-bucket --style sage \
		> $@

$(ART)/formulas/bm-extra.tex: latex_formulas.sage $(XLCOSTFORMULAS) | $(ART)/formulas
	$(SAGE) latex_formulas.sage \
		--formula-file $(XLCOSTFORMULAS) \
		--extra-BM --style factor \
		> $@

$(ART)/formulas/all-bit-ops.tex: latex_formulas.sage $(XLCOSTFORMULAS) $(XLTEST) | $(ART)/formulas
	$(SAGE) latex_formulas.sage \
		--formula-file $(XLCOSTFORMULAS) \
		--bit-costs --xl-test $(XLTEST) --style sage --inline-extra-BM \
		> $@

all-formulas: $(FORMULA_FILES)

# --------------------------------------------------------------------
# Figure data and figure snippets
# --------------------------------------------------------------------

$(SPLIT_SENTINEL): $(PRED_CSV) split_predictions.sh | $(SPLIT_PRED_DIR)
	bash split_predictions.sh $(PRED_CSV) $(SPLIT_PRED_DIR)
	touch $@

split-predictions: $(SPLIT_SENTINEL)

$(ART)/figures/quotient_convergence_grid.tex: quotient_convergence_grid_csv.py $(PRED_CSV) $(GET_D_FILES) | $(ART)/figures
	$(PYTHON) quotient_convergence_grid_csv.py \
		--csv $(PRED_CSV) \
		--get-d-dir $(DATA) \
		--output $@

all-figures: $(FIGURE_FILES) split-predictions

# --------------------------------------------------------------------
# Provenance metadata
# --------------------------------------------------------------------
#
# These files are intentionally regenerated when you explicitly ask for
# metadata or paper-artifacts. They are small and cheap. They should not
# trigger table/formula/figure regeneration.

$(ART)/CODE_VERSION: | $(ART)
	git rev-parse HEAD > $@

$(ART)/CODE_STATUS: | $(ART)
	git status --short > $@

metadata: $(METADATA_FILES)

# --------------------------------------------------------------------
# Top-level artifact target
# --------------------------------------------------------------------

paper-artifacts: all-tables all-formulas all-figures prediction-error-artifacts metadata

# --------------------------------------------------------------------
# Cleaning
# --------------------------------------------------------------------

clean-artifacts:
	rm -rf $(ART)/tables $(ART)/formulas $(ART)/figures

clean-data:
	rm -rf $(DATA)

clean-error-data:
	rm -rf $(ERROR_DATA)

