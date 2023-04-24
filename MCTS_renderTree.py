import graphviz
import json
import time
from datetime import date
# Define the tree structure as a JSON-like string
game = 0 #TODO: put in loop
turn = 0 #TODO: put in loop

path_in = r"C:\\Users\\Maxime\\Desktop\\Travail\\c\\MCST_Connect4\\out\\MCTStree"
path_game_move = str(game) + "_" + str(turn)
path_full = path_in + path_game_move + ".json"

with open(path_full, "r") as data:
        tree_json = data.read()
        tree = json.loads(tree_json)



# Define a recursive function to add nodes and edges to the graph
def add_nodes(dot, node, parent=None, parent_name='', size=0.01, fontsize=10, penwidth=1, theta=0):
    # Define the label and color for the current node
    ratio = 0
    if(node['exploration'] != 0):
         ratio = node['value'] / node['exploration']
    r = int((1 - ratio) * 127.5)
    g = int((ratio + 1) * 127.5)
    color = f"#{r:02X}{g:02X}00"

    if(parent_name == ''):
        node['name'] = '0, '
    else:
        node['name'] =  str(int(parent_name[0]) + 1) + parent_name[1:] + str(node['last_move'])

    label = f"{node['name']}\n{node['value']}\n{node['exploration']}"

    # Add the current node to the graph
    dot.node(node['name'], label=label, shape='circle', style=f'filled', fillcolor=color, width=str(size), fontsize=str(fontsize), penwidth=str(penwidth))

    # Add an edge to the parent node, if provided
    if parent:
        dot.edge(str(parent['name']), str(node['name']))

    # Recursively add nodes and edges for each child of the current node
    if int(node['name'][0]) < 5:
        n_children = len(node.get('children', []))
        if n_children > 0:
            child_theta = theta - (n_children - 1)/2 * 15
            for child in node['children']:
                add_nodes(dot, child, parent=node, parent_name=node['name'], size=size*0.3, fontsize=fontsize*0.8, penwidth=penwidth*0.5, theta=child_theta)

# Parse the JSON-like string to a dictionary
tree = json.loads(tree_json)

# Create a new graph
dot = graphviz.Digraph(engine='twopi')

start = time.time()
date_t = date.today()
# Add nodes and edges to the graph
add_nodes(dot, tree)
print("graph done, time =", time.time()-start)
# Render the graph as a PDF file
dot.render(f'out/tree {date_t}', format='svg')
print("render svg done, time =", time.time()-start)
dot.render(f'out/tree {date_t}', format='png')
print("render png done, time =", time.time()-start)
