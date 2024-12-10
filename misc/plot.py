import pandas as pd
import plotly.express as px

df = pd.read_csv('test.csv')

fig = px.line(df, y = 'v')
fig.show()
