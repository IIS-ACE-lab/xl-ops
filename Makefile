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

.PHONY: all build data tables formulas figures split-predictions \
        prediction-error-artifacts metadata paper-artifacts \
        clean-artifacts clean-data clean-error-data

all: paper-artifacts

# --------------------------------------------------------------------
# Directory targets
# --------------------------------------------------------------------

$(DATA):
	mkdir -p $@

$(ART):
	mkdir -p $@

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
	$(MAKE) -C src

build: $(XLTEST)

# --------------------------------------------------------------------
# Prediction CSV
# --------------------------------------------------------------------

$(PRED_CSV): $(XLTEST) sage/xl_predict_range.sage sage/xl_cost_compare.sage sage/xl_cost_formulas.sage | $(DATA)
	$(SAGE) sage/xl_predict_range.sage \
		--compare-file sage/xl_cost_compare.sage \
		--exe $(XLTEST) \
		--n-min $(N_MIN) \
		--n-max $(N_MAX) \
		--m-factor $(M_FACTOR) \
		--output $@

# --------------------------------------------------------------------
# get_D lookup files
# --------------------------------------------------------------------

$(DATA)/get_D-%.txt: sage/get_D.sage | $(DATA)
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
# Copy processed data into paper-artifacts/data
# --------------------------------------------------------------------

$(ART)/data/predictions-allq-m2n-n010-400.csv: $(PRED_CSV) | $(ART)/data
	cp $< $@

$(ART)/data/get_D-%.txt: $(DATA)/get_D-%.txt | $(ART)/data
	cp $< $@

# --------------------------------------------------------------------
# Paper tables
# --------------------------------------------------------------------

$(ART)/tables/fukuoka_table.tex: sage/generate_guessing_table.sage sage/xl_cost_compare.sage sage/xl_cost_formulas.sage $(XLTEST) | $(ART)/tables
	$(SAGE) sage/generate_guessing_table.sage \
		--compare-file sage/xl_cost_compare.sage \
		--exe $(XLTEST) \
		--guessing-output none \
		--fukuoka-output $@

$(ART)/tables/guessing_table.tex: sage/generate_guessing_table.sage sage/xl_cost_compare.sage sage/xl_cost_formulas.sage $(XLTEST) | $(ART)/tables
	$(SAGE) sage/generate_guessing_table.sage \
		--compare-file sage/xl_cost_compare.sage \
		--exe $(XLTEST) \
		--guessing-output $@

$(ART)/tables/op-costs.tex: sage/xl_latex_formulas.sage sage/xl_cost_formulas.sage $(XLTEST) | $(ART)/tables
	$(SAGE) sage/xl_latex_formulas.sage \
		--op-cost \
		--xl-test $(XLTEST) \
		> $@


tables: $(TABLE_FILES) $(PAPER_DATA_FILES)

# --------------------------------------------------------------------
# Optional prediction-error artifacts
# --------------------------------------------------------------------

$(ERROR_DATA_STAMP): scripts/gen_prediction_error_data.sh sage/xl_cost_compare.sage sage/xl_cost_formulas.sage $(XLTEST)
	mkdir -p $(ERROR_DATA)
	bash scripts/gen_prediction_error_data.sh \
		--out-dir $(ERROR_DATA) \
		--sage $(SAGE) \
		--compare sage/xl_cost_compare.sage \
		--xl-test $(XLTEST) \
		--jobs 90% \
		--repeats 100

$(ERROR_TABLE) $(ERROR_FIGURE): scripts/analyze_prediction_errors.py $(ERROR_DATA_STAMP) | $(ART)/tables $(ART)/figures
	$(PYTHON) scripts/analyze_prediction_errors.py \
		--input-dir $(ERROR_DATA) \
		--table-output $(ERROR_TABLE) \
		--figure-output $(ERROR_FIGURE) \
		--quiet

prediction-error-artifacts: $(ERROR_TABLE) $(ERROR_FIGURE)

# --------------------------------------------------------------------
# LaTeX formula snippets
# --------------------------------------------------------------------

$(ART)/formulas/table-I.tex: sage/xl_latex_formulas.sage sage/xl_cost_formulas.sage | $(ART)/formulas
	$(SAGE) sage/xl_latex_formulas.sage \
		--table-I --style single-fraction \
		> $@

$(ART)/formulas/table-II.tex: sage/xl_latex_formulas.sage sage/xl_cost_formulas.sage | $(ART)/formulas
	$(SAGE) sage/xl_latex_formulas.sage \
		--table-II --style sage \
		> $@

$(ART)/formulas/op-formulas-base.tex: sage/xl_latex_formulas.sage sage/xl_cost_formulas.sage | $(ART)/formulas
	$(SAGE) sage/xl_latex_formulas.sage \
		--all-base --style sage \
		> $@

$(ART)/formulas/op-formulas-const.tex: sage/xl_latex_formulas.sage sage/xl_cost_formulas.sage | $(ART)/formulas
	$(SAGE) sage/xl_latex_formulas.sage \
		--all-const --style sage \
		> $@

$(ART)/formulas/op-formulas-const-bucket.tex: sage/xl_latex_formulas.sage sage/xl_cost_formulas.sage | $(ART)/formulas
	$(SAGE) sage/xl_latex_formulas.sage \
		--all-const-bucket --style sage \
		> $@

$(ART)/formulas/bm-extra.tex: sage/xl_latex_formulas.sage sage/xl_cost_formulas.sage | $(ART)/formulas
	$(SAGE) sage/xl_latex_formulas.sage \
		--extra-BM --style factor \
		> $@

$(ART)/formulas/all-bit-ops.tex: sage/xl_latex_formulas.sage sage/xl_cost_formulas.sage $(XLTEST) | $(ART)/formulas
	$(SAGE) sage/xl_latex_formulas.sage \
		--bit-costs --xl-test $(XLTEST) --style sage --inline-extra-BM \
		> $@

formulas: $(FORMULA_FILES)

# --------------------------------------------------------------------
# Figure data and figure snippets
# --------------------------------------------------------------------

$(SPLIT_SENTINEL): $(PRED_CSV) scripts/split_predictions.sh | $(SPLIT_PRED_DIR)
	bash scripts/split_predictions.sh $(PRED_CSV) $(SPLIT_PRED_DIR)
	touch $@

split-predictions: $(SPLIT_SENTINEL)

$(ART)/figures/quotient_convergence_grid.tex: scripts/quotient_convergence_grid_csv.py $(PRED_CSV) $(GET_D_FILES) | $(ART)/figures
	$(PYTHON) scripts/quotient_convergence_grid_csv.py \
		--csv $(PRED_CSV) \
		--get-d-dir $(DATA) \
		--output $@

figures: $(FIGURE_FILES) split-predictions

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

paper-artifacts: tables formulas figures prediction-error-artifacts metadata

# --------------------------------------------------------------------
# Cleaning
# --------------------------------------------------------------------

clean-artifacts:
	rm -rf $(ART)

clean-data:
	rm -rf $(DATA)

clean-error-data:
	rm -rf $(ERROR_DATA)

