# `XL_cost.sage`

`XL_cost.sage` predicts the concrete bit-operation cost of XL with
Wiedemann linear algebra and Berlekamp--Massey for a given field size and
parameter set.

## Requirements

Build `XL_test` first, for example:

```sh
make -C src
```

The examples below assume that the executable is available as:

```sh
src/bin/XL_test
```

## Basic prediction

Supported fields are `q = 2`, `q = 31`, and `q = 256`.

Run the tool with `--pred` and provide the field size `q`, number of variables
`n`, and number of equations `m`:

```sh
sage sage/XL_cost.sage \
  --exe src/bin/XL_test \
  -q 256 -n 72 -m 74 \
  --pred --pretty
```

The `--pretty` flag prints a human-readable summary, including base-2 logarithms
of the predicted bit-operation costs.

## Variants

Baseline model:

```sh
sage sage/XL_cost.sage \
  --exe src/bin/XL_test \
  -q 256 -n 72 -m 74 \
  --pred --pretty
```

Const model:

```sh
sage sage/XL_cost.sage \
  --exe src/bin/XL_test \
  -q 256 -n 72 -m 74 \
  --pred --pretty -c
```

Const+bucket model:

```sh
sage sage/XL_cost.sage \
  --exe src/bin/XL_test \
  -q 256 -n 72 -m 74 \
  --pred --pretty -c -b
```

