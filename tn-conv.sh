#!/bin/sh
trans $0/../tn-train <$1 | paps --lpi=6 --cpi=10 --paper=letter --format=pdf --top-margin=10 --bottom-margin=11 --font=Menlo --trunc > $1.pdf
