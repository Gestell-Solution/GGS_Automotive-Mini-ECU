# Gestell Automotive Mini-ECU — Developer Workflow Reference

> **Document Type:** Team Reference Guide
> **Audience:** All Teams (Embedded · Dashboard · Mobile)
> **Last Updated:** 2026-10-12
> **Status:** 🟢 Active

---

## Table of Contents

1. [Overview](#1-overview)
2. [Branch Naming Rules](#2-branch-naming-rules)
3. [Step-by-Step Task Workflow](#3-step-by-step-task-workflow)
4. [Commit Message Format](#4-commit-message-format)
5. [Docker Environment Setup](#5-docker-environment-setup)
6. [Docker Daily Usage](#6-docker-daily-usage)
7. [Quick Reference Card](#7-quick-reference-card)

---

## 1. Overview

The project uses a **Single-Repository, Multi-Branch** strategy. Each team works in isolation on its own development branch and merges through Pull Requests only.

```
Master  ──────────────────────────────────── (Production — Protected)
   │
   ├─── em_develop  ──────────── (Embedded Team — Protected)
   │         └─── Feature/62-em-atmega128-registers  ← Developer works here
   │
   ├─── da_develop  ──────────── (Dashboard Team — Protected)
   │         └─── feature/XX-da-feature-name
   │
   └─── mo_develop  ──────────── (Mobile Team — Protected)
             └─── feature/XX-mo-feature-name
```

| Branch | Purpose | Direct Push? |
|:---|:---|:---:|
| `Master` | Production-ready releases | No — PR only |
| `em_develop` | Embedded integration | No — PR only |
| `da_develop` | Dashboard integration | No — PR only |
| `mo_develop` | Mobile integration | No — PR only |
| `Feature/*/Bug/*/Test/*` | Active development | Yes |
| `hotfix/*` / `Hotfix/*` | Critical production fixes | Yes — then PR to Master |

---

## 2. Branch Naming Rules

All branch names **must** match this regex pattern (enforced by GitHub branch protection):

```
^(Master|em_develop|da_develop|mo_develop)$
|^(feature|Feat|bug|Bug|test|Test)/[0-9]+-(em|da|mo)-[a-zA-Z0-9\-]+$
|^(hotfix|Hotfix)/[0-9]+-[a-zA-Z0-9\-]+$
```

### Pattern Breakdown

| Type | Format | Example |
|:---|:---|:---|
| Feature branch | `feature/<issue#>-<team>-<description>` | `feature/62-em-atmega128-registers` |
| Feature branch (Alt) | `Feat/<issue#>-<team>-<description>` | `Feat/62-em-register-map` |
| Bug fix branch | `bug/<issue#>-<team>-<description>` | `bug/75-em-uart-overflow` |
| Test branch | `test/<issue#>-<team>-<description>` | `test/80-em-adc-coverage` |
| Hotfix branch | `hotfix/<issue#>-<description>` | `hotfix/90-critical-pwm-crash` |

> **Note:** The issue number must match an existing GitHub Issue number.
> **Team codes:** `em` = Embedded · `da` = Dashboard · `mo` = Mobile

---

## 3. Step-by-Step Task Workflow

This is the **exact sequence of steps** to follow for every task from start to finish.

### Step 1 — Sync Your Base Branch

Before starting any new work, make sure your local develop branch is up to date:

```bash
# Embedded developers:
git checkout em_develop
git pull origin em_develop

# Dashboard developers:
git checkout da_develop
git pull origin da_develop

# Mobile developers:
git checkout mo_develop
git pull origin mo_develop
```

### Step 2 — Create Your Feature Branch

Create a new branch from your team's develop branch.
The branch name **must** follow the naming convention in Section 2.

```bash
# Syntax:
git checkout -b <branch-name>

# Real example (US-000 — ATmega128 Registers task):
git checkout -b Feature/62-em-atmega128-registers
```

Verify you are on the correct branch:

```bash
git branch
# Expected: * Feature/62-em-atmega128-registers
```

### Step 3 — Push the Branch to Remote Immediately

Push the new (empty) branch so the team can see it exists:

```bash
git push origin Feature/62-em-atmega128-registers
```

### Step 4 — Do Your Work (Edit → Stage → Commit Loop)

Work in small, focused commits. Each commit should represent one logical change.

```bash
# 1. Make your edits in your editor

# 2. Check what changed:
git status
git diff

# 3. Stage only the relevant files:
git add Embedded/Firmware/Src/Mcal/Atmega128Register.h

# 4. Commit with a proper message (see Section 4):
git commit -m "feat(mcal) : Add helper macros _SetAddress8Bit and _SetAddress16Bit"

# 5. Push the commit:
git push origin Feature/62-em-atmega128-registers
```

> **Important:** Files listed in `.gitignore` cannot be committed.
> In this project, `AgileManagment/*` is intentionally excluded from git.
> Management files (User Stories, Agile Plan) live locally only — not in the repository.

### Step 5 — Keep Your Branch Up to Date

If `em_develop` gets new commits while you are working, rebase to stay current and avoid conflicts:

```bash
git fetch origin
git rebase origin/em_develop
```

### Step 6 — Open a Pull Request (PR)

When your task is complete:

1. Go to the GitHub repository page.
2. Click **"Compare & pull request"** for your branch.
3. Set the **base branch** to your team's develop branch (`em_develop`, `da_develop`, or `mo_develop`).
4. Fill in the PR title: `[EMBEDDED] Brief description`
   - Example: `[EMBEDDED] Complete ATmega128 Register Map — US-000`
5. Add a description referencing the User Story: `Closes US-000`
6. Request at least **1 reviewer** from your team.
7. **Wait for all CI/CD checks to pass (green)** before requesting review.

### Step 7 — After Merge (Cleanup)

Once your PR is merged, clean up your local workspace:

```bash
# Switch back to develop:
git checkout em_develop

# Pull the merged changes:
git pull origin em_develop

# Delete the local feature branch:
git branch -d Feature/62-em-atmega128-registers

# (Optional) Delete the remote branch if GitHub did not auto-delete:
git push origin --delete Feature/62-em-atmega128-registers
```

---

## 4. Commit Message Format

Every commit must follow this exact format:

```
<type>(<scope>) : <subject>

<optional body — explain WHY, not WHAT>
```

### Types

| Type | When to Use |
|:---:|:---|
| `feat` | Adding a new feature or implementation |
| `fix` | Fixing a bug |
| `docs` | Documentation changes only |
| `style` | Formatting, whitespace (no logic change) |
| `refactor` | Code restructuring without changing behavior |
| `test` | Adding or updating tests |
| `chore` | Build system, config, tooling changes |

### Scope Examples by Team

| Team | Common Scopes |
|:---:|:---|
| Embedded | `mcal`, `hal`, `os`, `app`, `embedded` |
| Dashboard | `dashboard`, `ui`, `api` |
| Mobile | `mobile`, `flutter`, `bt` |

### Real Examples from This Project

```bash
# First commit — adding the helper macros only:
git commit -m "feat(mcal) : Add the Helper macros _SetAddress8Bit(Address) and _SetAddress16Bit(Address) are defined."

# Final commit — completing the full register map with body:
git commit -m "feat(mcal) : Complete ATmega128 register map — US-000

- Add all DIO port registers (A-G) with Classic and Extended I/O addresses
- Add ADC registers (ADMUX, ADCSRA, ADCH/L, ADCSRB) with bit positions
- Add Timer0/1/2/3 registers with bit-position macros
- Add EXTI registers (EICRA, EICRB, EIMSK, EIFR) for INT4/INT5
- Add USART0 (Bluetooth HC-05) and USART1 (WiFi ESP-01) registers
- Add SPI, TWI/I2C, EEPROM, and CPU core registers
- All addresses verified against ATmega128 datasheet Rev. 2467X-AVR-06/11

Closes US-000 — All acceptance criteria satisfied."
```

---

## 5. Docker Environment Setup

The project ships a pre-configured Docker environment.
**Do not install AVR, Node.js, or Flutter locally** — use Docker so everyone runs the exact same toolchain version.

### What Is Inside the Docker Image?

| Tool | Version | Used By |
|:---|:---:|:---|
| `avr-gcc` | 5.4.0 | Embedded — firmware compilation |
| `avrdude` | 6.3 | Embedded — firmware flashing |
| `gcc-13` / `gcov-13` | 13.x | Embedded — unit tests + coverage |
| `cppcheck` | 2.x | Embedded — MISRA-C static analysis |
| `lcov` + `genhtml` | 1.14+ | Embedded — HTML coverage reports |
| `node` | 20.x LTS | Dashboard team |
| `npm` | 10.x | Dashboard team |
| `flutter` | stable | Mobile team |

### Two Security Tokens

The Docker setup uses two separate tokens for access control:

| Token | Where Used | Purpose |
|:---|:---:|:---|
| `BUILD_TOKEN` | Build time | Required to build the Docker image — prevents unauthorized image creation |
| `GESTELL_TOKEN` | Run time | Required to start a container shell — authenticates the developer |

Both token values are provided by the team lead. **Never commit them to git.**

### Prerequisites

- [Docker Desktop](https://www.docker.com/products/docker-desktop/) installed (or Docker Engine on Linux)
- Both tokens obtained from your team lead

### First-Time Setup — Build the Image

Run this **once** when you first clone the repository (or after the Dockerfile changes):

```bash
# Navigate to the project root:
cd /path/to/Automotive-Mini-ECU

# Build the shared image:
docker compose build --build-arg BUILD_TOKEN=<secret>
```

> This takes several minutes on first run because it downloads and installs
> all toolchains including the Flutter SDK (~1 GB).

The built image is named `gestell-ecu-dev` and is shared by all three services
(embedded, dashboard, mobile).

### Set the Runtime Token

Export the runtime token in your shell **once per terminal session**:

```bash
export GESTELL_TOKEN=<secret>
```

After this, all `docker compose run` commands in the same terminal session
will automatically have the token available.

---

## 6. Docker Daily Usage

### Opening an Interactive Shell

```bash
# Embedded Team (working directory inside container: /workspace/Embedded):
docker compose run --rm embedded

# Dashboard Team (working directory: /workspace/Dashboard):
docker compose run --rm dashboard

# Mobile Team (working directory: /workspace/MobileApp):
docker compose run --rm mobile
```

Inside the container, the entire project directory is mounted at `/workspace`,
so any files you edit on your host machine are immediately visible inside the container.

### Running Build Commands Without Opening a Shell

```bash
# Compile the firmware:
docker compose run --rm embedded make all

# Run unit tests:
docker compose run --rm embedded make test

# Generate code coverage HTML report:
docker compose run --rm embedded make coverage

# Run MISRA-C static analysis (cppcheck):
docker compose run --rm embedded make misra

# Show firmware binary size:
docker compose run --rm embedded make size

# Clean all build artifacts:
docker compose run --rm embedded make clean
```

### CI / GitHub Actions Mode

In CI pipelines, set `GESTELL_CI=true` to bypass the interactive token check:

```bash
GESTELL_CI=true docker compose run --rm embedded make all
```

### Stopping and Cleaning Up

```bash
# Stop all running containers gracefully:
docker compose down

# Remove the built image (forces a fresh build next time):
docker rmi gestell-ecu-dev

# Full cleanup — remove containers, networks, and image:
docker compose down --rmi all
```

### Rebuilding After Dockerfile Changes

When the team lead announces that the `Dockerfile` or `docker-compose.yml` was updated:

```bash
docker compose build --build-arg BUILD_TOKEN=<secret> --no-cache
```

---

## 7. Quick Reference Card

### Git Commands

```bash
# SETUP ───────────────────────────────────────────────────────────────────

# Sync your team's develop branch before starting a task:
git checkout em_develop
git pull origin em_develop

# Create and publish the feature branch:
git checkout -b Feature/62-em-atmega128-registers
git push origin Feature/62-em-atmega128-registers

# WORK LOOP ───────────────────────────────────────────────────────────────

git status                                        # See changed files
git diff                                          # See exact line changes
git add Embedded/Firmware/Src/Mcal/File.h         # Stage specific file
git commit -m "feat(mcal) : <subject>"            # Commit
git push origin Feature/62-em-atmega128-registers # Push to remote

# STAYING CURRENT ─────────────────────────────────────────────────────────

git fetch origin
git rebase origin/em_develop                      # Rebase on latest develop

# AFTER MERGE CLEANUP ─────────────────────────────────────────────────────

git checkout em_develop
git pull origin em_develop
git branch -d Feature/62-em-atmega128-registers   # Delete local branch
```

### Docker Commands

```bash
# FIRST TIME SETUP ────────────────────────────────────────────────────────

export GESTELL_TOKEN=<secret>
docker compose build --build-arg BUILD_TOKEN=<secret>

# DAILY USAGE — EMBEDDED TEAM ─────────────────────────────────────────────

docker compose run --rm embedded                  # Open interactive shell
docker compose run --rm embedded make all         # Build firmware
docker compose run --rm embedded make test        # Run unit tests
docker compose run --rm embedded make coverage    # Coverage report
docker compose run --rm embedded make misra       # Static analysis
docker compose run --rm embedded make size        # Binary size report
docker compose run --rm embedded make clean       # Clean build artifacts

# DAILY USAGE — OTHER TEAMS ───────────────────────────────────────────────

docker compose run --rm dashboard                 # Dashboard shell
docker compose run --rm mobile                    # Mobile shell

# MAINTENANCE ─────────────────────────────────────────────────────────────

docker compose down                               # Stop containers
docker compose build --build-arg BUILD_TOKEN=<secret> --no-cache  # Rebuild image
docker compose down --rmi all                     # Full cleanup
```

---

> **Questions?** Contact your team lead or open an issue in the repository.
>
> **Copyright © 2026 Gestell Company — All Rights Reserved**
