# parallelDistributedECG
Parallel and Distributed Learning for Scalable, Personalized Real-Time ECG Monitoring

**Research Area:** This work is positioned at the crossroads of parallel and distributed computing, federated learning, and individual-centered medicine. Its goal is to create scalable, real-time ECG monitoring systems employing sophisticated deep learning techniques. This work contributes to important problems from the perspective of personalized medicine such as data privacy, low-power, low-latency, and less-than-adept medical device processing, as well as large scale distributed ECG signal processing.

**Objectives and Expected Contributions:** The aim is to create a scalable distributed architecture that combines parallel computing with federated and self-supervised learning for real-time ECG monitoring. This architecture will emphasize model sync across the decentralized nodes, minimizing the annotation workload, and improving the resource efficiency on constrained devices. Anticipated results comprise of a new framework for privacy-friendly, individualized ECG monitoring, innovative techniques to lower the essential communication for accurate diagnoses while retaining the essential high sensitivity and specificity, and analytical assessments revealing the disparity between the efficiency of the algorithms and the accuracy of the models.

# ECG Processing Sequential Framework

This project implements a baseline sequential version of a framework for real-time ECG monitoring using simulated ECG data. The purpose is to establish a performance baseline before parallelizing the code.

## Build Instructions

This project uses CMake. To build:
```bash
mkdir build
cd build
cmake ..
make
