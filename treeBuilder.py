from graphviz import Digraph

filename = "thompson.txt"
with open(filename) as f:
    content = f.readlines()

f = Digraph('finite_state_machine', filename='thompson.gv')
f.attr(rankdir='LR', size='8,5')
f.attr('node', shape='doublecircle')
f.node('9999')
        
f.attr('node', shape='circle')

for line in content:
    edge = line.split(" ")
    f.edge(edge[0], edge[1], label=edge[2])

f.view()

filename = "afd.txt"
with open(filename) as f:
    content = f.readlines()

f = Digraph('finite_state_machine', filename='afd.gv')
f.attr(rankdir='LR', size='8,5')
        
f.attr('node', shape='doublecircle')
for line in content:
    edge = line.split(" ")
    if (len(edge) == 1):
        f.node(edge[0])

f.attr('node', shape='circle')

for line in content:
    edge = line.split(" ")
    if (len(edge) == 3):
        f.edge(edge[0], edge[1], label=edge[2])

f.view()

filename = "afdirect.txt"
with open(filename) as f:
    content = f.readlines()

f = Digraph('finite_state_machine', filename='afdirect.gv')
f.attr(rankdir='LR', size='8,5')

#f.attr('node', shape='doublecircle')
#for line in content:
#    edge = line.split(" ")
#    if (len(edge) == 1):
#        f.node(edge[0])

f.attr('node', shape='circle')

for line in content:
    edge = line.split(" ")
    if (len(edge) == 3):
        f.edge(edge[0], edge[1], label=edge[2])

f.view()
