#!/usr/bin/env python

import os
import pydot
from optparse import OptionParser

class SmaliCG:
    def __init__(self, root):
        self.root = root
        self.cg = {}
        self.ctree = {}
        self.generateCG()

    def generateCG(self):
        for root, dirs, files in os.walk(self.root):
            for file in files:
                full = os.path.join(root, file)

                if not full.endswith(".smali"):
                    continue

                cur_class = ''
                cur_method = ''
                for l in open(full, 'r').readlines():
                    l = l.strip()
                    if l.startswith(".class"):
                        cur_class = l.split(' ')[-1]
                    elif l.startswith(".method"):
                        cur_method = l.split(' ')[-1]
                    elif l.startswith("invoke-"):
                        invoke_target = l.split(' ')[-1]

                        key = (cur_class, cur_method)
                        if key in self.cg:
                            self.cg[key].append(invoke_target)
                        else:
                            self.cg[key] = [invoke_target]

    def findDirectlyInvokeTo(self, api):
        invokers = []
        for key in self.cg:
            for invoke_target in self.cg[key]:
                if invoke_target.find(api) != -1:
                    caller = key[0] + '->' + key[1]
                    if caller not in invokers:
                        invokers.append(caller)
        return invokers

    def findInvokeTo(self, api, maxLevel):
        task_set = [api]
        for l in range(maxLevel):
            for mtd in task_set:
                if mtd in self.ctree.keys():
                    continue

                invokers = self.findDirectlyInvokeTo(mtd)
                self.ctree[mtd] = invokers
                task_set = task_set + invokers

    def addNode(self, name):
        node = pydot.Node(name, style="filled", shape="box", color="#cccccc", fontname="Sans", fontsize=8)
        self.graph.add_node(node)
        return node

    def generateGraph(self, png):
        self.graph = pydot.Dot(graph_type='digraph', shape="record", simplify=True)
        node_table = {}

        for key in self.ctree.keys():
            if key not in node_table:
                node_table[key] = self.addNode(key)

            for caller in self.ctree[key]:
                if caller not in node_table:
                    node_table[caller] = self.addNode(caller)

                self.graph.add_edge(pydot.Edge(node_table[caller], node_table[key], weight=1))

        self.graph.write_png(png)


def main():
    usage = """Usage: %prog <api_list> [-r <root>] [-p <png>] [-l <level>]"""
    parser = OptionParser(usage=usage, version='Smali Calling Graph Grep v0.1')
    parser.add_option('-r', '--root', metavar='<path>', help='Root directory of smali codes, default is current')
    parser.add_option('-p', '--png', metavar='<path>', help='Generated PNG file, default is a temp file')
    parser.add_option('-l', '--level', metavar='<number>', default='5', help='Backtrace depth, default is 5')
    (opts, args) = parser.parse_args()

    if len(args) == 0:
        print 'ERROR: no API inputted'
        return

    if opts.root == None:
        root = os.getcwd()
    else:
        root = opts.root

    scg = SmaliCG(root)

    for api in args:
        if opts.png == None:
            import tempfile
            n = api.split('->')[-1].split('(')[0]
            png = '%s_%s.png' % (tempfile.mkstemp()[1], n)
        else:
            png = opts.png

        scg.findInvokeTo(api, int(opts.level))
        scg.generateGraph(png)
        os.system('open -a "Preview" ' + png)

if __name__=='__main__':
    main()