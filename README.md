# EDF Scheduler Implementation in FreeRTOS

## Introduction
This repository contains an implementation of the Earliest Deadline First (EDF) scheduling algorithm for FreeRTOS, aimed at enhancing task scheduling based on deadlines. The EDF Scheduler is designed to ensure that tasks with the nearest deadlines are given higher priority, making it highly suitable for real-time systems where meeting deadlines is critical.

## Features
- **EDF Scheduling:** Implements the EDF algorithm in FreeRTOS to manage task priorities based on deadlines.
- **Dynamic Priority Adjustment:** Automatically adjusts task priorities as time progresses and deadlines approach.
- **Compatibility:** Designed to be integrated with existing FreeRTOS projects with minimal modifications required.
