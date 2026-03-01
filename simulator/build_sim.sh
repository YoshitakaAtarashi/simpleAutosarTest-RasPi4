#!/bin/bash
#
# build_sim.sh – Build the Raspberry Pi 4 AUTOSAR simulator for Linux / macOS
#
# Requirements:
#   gcc (any modern version, no cross-compiler needed)
#
# Usage:
#   cd simulator
#   bash build_sim.sh          # build
#   bash build_sim.sh run      # build and run (10-second simulation)
#   bash build_sim.sh clean    # remove build artefacts
#

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
APP_DIR="$SCRIPT_DIR/../app"
OUTPUT="$SCRIPT_DIR/sim_app"

# ---- Clean ----
if [ "${1}" = "clean" ]; then
    rm -f "$OUTPUT"
    echo -e "${GREEN}Cleaned.${NC}"
    exit 0
fi

# ---- Check GCC ----
if ! command -v gcc &> /dev/null; then
    echo -e "${RED}Error: gcc not found.${NC}"
    echo -e "${YELLOW}Install it with:  sudo apt-get install gcc   (Debian/Ubuntu)${NC}"
    echo -e "${YELLOW}                  brew install gcc            (macOS)${NC}"
    exit 1
fi

echo -e "${GREEN}================================================${NC}"
echo -e "${GREEN}  Trampoline AUTOSAR OS – PC Simulator Build   ${NC}"
echo -e "${GREEN}================================================${NC}"
echo ""
echo "  App dir : $APP_DIR"
echo "  Compiler: $(gcc --version | head -n1)"
echo ""

# ---- Compile ----
#   * SIMULATOR    – marks a simulator build (can be used for guards)
#   * SIM_DURATION_SEC=10 – run simulation for 10 seconds then exit
#     Remove this flag to run indefinitely (stop with Ctrl+C).
gcc -O2 -Wall -Wextra \
    -DSIMULATOR \
    -DSIM_DURATION_SEC=10 \
    -I"$APP_DIR" \
    "$APP_DIR/main.c" \
    "$SCRIPT_DIR/sim_os.c" \
    "$SCRIPT_DIR/sim_drivers.c" \
    -o "$OUTPUT"

echo -e "${GREEN}Build successful: $OUTPUT${NC}"
echo ""

# ---- Run ----
if [ "${1}" = "run" ]; then
    echo -e "${YELLOW}Running simulator (10 seconds)...${NC}"
    echo "---"
    "$OUTPUT"
fi
