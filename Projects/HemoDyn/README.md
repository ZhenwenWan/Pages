# Hybrid Simulation–Data Regression Framework
**Instantiated Example: HemoDyn – Physiological Model–Informed Neural Networks for Hemodynamics**

## Overview
This framework models and predicts how complex systems change over time by combining **mechanistic simulation** with **observational data** in a regression workflow.  
It is adaptable across domains — from industrial processes to environmental systems to biomedical applications.  
In this repository, we demonstrate its use in **hemodynamics** through the **HemoDyn** project.

---

## Core Methodology
1. **Mechanistic Base Model** – Start with a validated physical or physiological model.  
2. **Parameter Generalization** – Introduce tunable parameters representing uncertainty.  
3. **Hybrid Learning** – Fit these parameters to real-world measurements using regression or machine learning.  
4. **Physics + Data Fusion** – Use data assimilation to keep the model updated with new observations.  
5. **Interpretability** – Maintain a parameter set that maps directly to physical or physiological meaning.  

This is a **compact alternative to black-box deep learning**, requiring fewer parameters and less data, while retaining flexibility missing in rigid simulations.

---

## The HemoDyn Example
In HemoDyn, the framework becomes **PMINNs** (Physiological Model–Informed Neural Networks) for modeling **lower extremity blood flow and oxygen transport**.

- **Reduced-Order CFD Solver** (SimVascular ROM) for arterial blood flow.  
- **Oxygen Transport Simulation** (Elmer Multiphysics) for delivery, exchange, and consumption.  
- **Wearable Integration** – Pulse and SpO₂ data from smartwatches and oximeters.  
- **Machine Learning Layer** – Neural network estimates physiological parameters within constraints.  

This architecture enables **clinical interpretability** while improving efficiency and reducing data needs.

---

## General Workflow
```python
# 1. Import domain-specific model
from hemodyn.model import HemodynamicsModel

# 2. Load initial parameters and sensor data
model = HemodynamicsModel(params_init)
data = load_wearable_data("pulse_oximeter.csv")

# 3. Run mechanistic simulation
sim_results = model.run()

# 4. Perform hybrid regression update
from hybrid_regression import fit_model
updated_model = fit_model(model, data)

# 5. Predict and visualize
predictions = updated_model.predict(time_window="next_24h")
plot_results(predictions)
```

---

## PMINNs Mapping to Framework API
1. **Step 1 – Mechanistic Simulation**  
   Implement a physics-based blood flow and oxygen transport solver.  

2. **Step 2 – Data Integration**  
   Ingest wearable device data (Pulse, SpO₂) and pre-process it.  

3. **Step 3 – Hybrid Regression Update**  
   Use regression or neural network fitting constrained by physiological models.  

4. **Step 4 – Output & Interpretation**  
   Provide predictions and insights clinicians can interpret directly.

---

## Adapting to Other Domains
To use the framework beyond hemodynamics:
1. Replace the **mechanistic model** with your domain model (e.g., HVAC, climate, battery systems).  
2. Identify key **tunable parameters**.  
3. Set up a **data assimilation pipeline** for real-time updates.  
4. Implement a **diagnostic interface** to interpret results.  

---

## Why Use This Framework?
- **Cross-domain adaptability**  
- **Lower data requirements** than black-box models  
- **Interpretability** via meaningful parameters  
- **Real-time adaptability** with incoming data  
- **Computational efficiency** for deployment in clinical, industrial, or IoT environments  

---

## Potential Applications
- Clinical decision support (e.g., vascular disease monitoring)  
- Industrial fault detection  
- Climate and environmental forecasting  
- Engineering system optimization  
