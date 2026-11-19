# 1-Week Intensive Roadmap: Embedded C Unit Testing & Unity Framework

This study plan is designed for Embedded Systems Engineers looking to master **Unit Testing in C** using the **Unity Framework** and the **AAA (Arrange-Act-Assert)** pattern. The plan relies exclusively on authoritative technical books.

---

## 🎯 Overall Course Objectives

By the end of this 7-day plan, participants will be able to:
1. **Understand Embedded Unit Testing Principles**: Distinguish between Host-Based (PC) testing and Target-Based (MCU) testing.
2. **Master the AAA Pattern**: Write clean, maintainable, and structured test cases using **Arrange**, **Act**, and **Assert**.
3. **Utilize Unity Test Framework**: Set up, write, and execute unit tests for standalone C modules.
4. **Isolate Hardware Dependencies**: Use Stubs, Fakes, and Mocks (via CMock) to isolate hardware registers, HAL drivers, and peripherals.
5. **Integrate Test Build Systems**: Automate unit test execution using Makefiles or Ceedling and measure code coverage.

---

## 📚 Required Textbooks

* **[Book 1 - Primary]** *Test-Driven Development for Embedded C* — **James W. Grenning**
* **[Book 2 - Core Patterns]** *Unit Testing Principles, Practices, and Patterns* — **Vladimir Khorikov** *(Focus on AAA pattern & test architecture)*
* **[Book 3 - Advanced Techniques]** *Working Effectively with Legacy Code* — **Michael C. Feathers** *(Focus on refactoring and seam creation)*

---

## 🗓️ 7-Day Detailed Breakdown

### Day 1: Introduction to Embedded Unit Testing & the AAA Pattern
* **Objective**: Understand Host-Based testing logic and structure your very first test case using the AAA standard.
* **Key Topics**:
  * Host-Based Testing vs. Target-Based Testing.
  * The AAA Pattern Structure:
    * **Arrange**: Set up pre-conditions, inputs, and environment.
    * **Act**: Execute the function under test.
    * **Assert**: Verify outputs, state changes, and return values.
* **Book Reading**:
  * **Grenning**: *Chapter 1 (Test-Driven Development) & Chapter 2 (TDD in a Nutshell).*
  * **Khorikov**: *Chapter 3 (The Anatomy of a Unit Test - AAA Pattern section).*
* **Practical Exercise**:
  * Take a basic C function (e.g., a Ring Buffer length calculator) and draft its AAA test steps on paper or inside code comments.

---

### Day 2: Getting Started with Unity Test Framework
* **Objective**: Configure Unity Framework and build a minimal test harness in C.
* **Key Topics**:
  * Structure of Unity (`unity.c` and `unity.h`).
  * Test execution lifecycle: `setUp()` and `tearDown()`.
  * Common Unity Assertions (`TEST_ASSERT_EQUAL`, `TEST_ASSERT_TRUE`, `TEST_ASSERT_EQUAL_HEX8`, `TEST_ASSERT_EQUAL_MEMORY`).
* **Book Reading**:
  * **Grenning**: *Chapter 3 (Starting a C Module with TDD) & Appendix A (Unity User Guide).*
* **Practical Exercise**:
  * Create a local C project, include `unity.h`, and write a basic test suite for an LCD display controller module (pure logic, no hardware calls).

---

### Day 3: Deep Dive into AAA & Advanced Unity Assertions
* **Objective**: Master data type assertions and handle edge cases safely.
* **Key Topics**:
  * Selecting appropriate assertions (Bitwise checks, Arrays, Pointer verification, Enums).
  * Testing Boundary Conditions & Edge Cases (Overflow, Underflow, Null Pointers, Invalid Arguments).
  * Keeping Arrange, Act, and Assert strictly separated to prevent test leakage.
* **Book Reading**:
  * **Khorikov**: *Chapter 3 (Best Practices for AAA Pattern & Naming Conventions).*
  * **Grenning**: *Chapter 4 (Testing Modules with State).*
* **Practical Exercise**:
  * Write unit tests for a Circular Buffer driver, testing overflow/underflow scenarios using strict AAA formatting.

---

### Day 4: Hardware Isolation (Stubs & Fakes)
* **Objective**: Test high-level embedded logic without physical microcontroller hardware.
* **Key Topics**:
  * Test Double Definitions: Dummy, Stub, and Fake.
  * Creating Hardware Abstraction Layers (HAL) and Seams.
  * Writing Fake functions to simulate sensor reads and GPIO state changes.
* **Book Reading**:
  * **Grenning**: *Chapter 6 (Flash Driver: First HW Dependency) & Chapter 7 (Faking the Hardware).*
* **Practical Exercise**:
  * Implement a unit test for a Sensor Data Processor module where `I2C_Read()` is replaced by a Fake function delivering test vectors.

---

### Day 5: Module Isolation with Mocks & CMock
* **Objective**: Automate dynamic mocking for embedded C modules.
* **Key Topics**:
  * Mocking vs. Faking: Verifying call counts, parameter values, and execution order.
  * Introduction to **CMock**: Automatically generating Mocks from C header (`.h`) files.
  * Integrating Unity and CMock.
* **Book Reading**:
  * **Grenning**: *Chapter 8 (Spying on the Hardware) & Chapter 9 (Collaborator Mocks).*
* **Practical Exercise**:
  * Create a Mock for a UART transmission driver and verify that `UART_SendByte()` is called with expected parameters and exact call frequencies.

---

### Day 6: Build System Automation & Code Coverage
* **Objective**: Automate test execution and evaluate test suite completeness.
* **Key Topics**:
  * Test runners and automation tools (**Ceedling** or custom `Makefile`).
  * Integrating unit tests into continuous local builds.
  * Measuring Code Coverage using `gcov` and `lcov`.
* **Book Reading**:
  * **Grenning**: *Chapter 12 (TDD Patterns and Anti-Patterns) & Chapter 13 (Refactoring Legacy Code).*
* **Practical Exercise**:
  * Organize your project into standard directories (`src/`, `test/`, `build/`) and write a `Makefile` that compiles and runs all Unity tests with a single command.

---

### Day 7: Capstone Mini-Project
* **Objective**: Consolidate all learned concepts into a complete embedded software module test suite.
* **Project Assignment**:
  * Build a **Temperature Control System** featuring:
    1. `ADC_Read()` (Isolated using a Stub/Mock).
    2. `Heater_SetState()` (Controlled based on temperature thresholds).
    3. `Alarm_Trigger()` (Invoked on over-temperature conditions).
  * Write 100% AAA-compliant unit tests in Unity.
  * Execute tests automatically and generate a Code Coverage report.
* **Book Reading**:
  * **Grenning**: *Chapter 14 (Conclusion & TDD Strategy).*

---

## 📌 Summary Checklist for AAA Pattern

| Phase | Responsibility | Rule of Thumb |
| :--- | :--- | :--- |
| **Arrange** | Prepare variables, mocks, stubs, and state. | Keep it clean; do not execute code under test here. |
| **Act** | Call the single function/method being tested. | Should ideally be 1 single line of code. |
| **Assert** | Verify expected outputs and state using Unity Macros. | Group relevant assertions together; fail fast. |