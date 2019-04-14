# libraries
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
 
# Data
df1=pd.read_csv('../exp1.csv', header=None, names=['Tempo (s)', 'Valor'])
df2=pd.read_csv('../exp2.csv', header=None, names=['Tempo (s)', 'Valor'])
df3=pd.read_csv('../exp3.csv', header=None, names=['Tempo (s)', 'Valor'])
df4=pd.read_csv('../exp4.csv', header=None, names=['Tempo (s)', 'Valor'])
df5=pd.read_csv('../exp5.csv', header=None, names=['Tempo (s)', 'Valor'])

plt.plot( 'Tempo (s)', 'Valor', data=df1, marker='', color='blue', linewidth=2, linestyle='dashed', label="Padrão")
plt.plot( 'Tempo (s)', 'Valor', data=df2, marker='', color='red', linewidth=2, linestyle='dashed', label="Padrão + First")
plt.plot( 'Tempo (s)', 'Valor', data=df3, marker='', color='orange', linewidth=2, linestyle='dashed', label="Padrão + Tenure")
plt.plot( 'Tempo (s)', 'Valor', data=df4, marker='', color='green', linewidth=2, label="Padrão + M1")
plt.plot( 'Tempo (s)', 'Valor', data=df5, marker='', color='black', linewidth=2, label="Padrão + M2")
plt.semilogx()
plt.legend()

plt.show()