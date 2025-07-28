#! /usr/bin/env python3
import sys
import json
import csv
import re


pe = re.compile(r'PkgType: (\S+)')

# with open('sbom.json') as f:
#    data = json.load(f)

data = json.load(sys.stdin)

components = data.get('packages', [])  # SPDXの場合は 'packages'


fieldnames = ['name', 'versionInfo', 'supplier', 'pkgtype']
writer = csv.DictWriter(sys.stdout, fieldnames=fieldnames)
writer.writeheader()

for comp in components:
    pkgtype = 'unknown'
    for annotation in comp.get('annotations'):
        comment = annotation.get('comment')
        match = pe.search(comment)
        if match:
            pkgtype = match.group(1)
    
    writer.writerow({
        'name': comp.get('name'),
        'versionInfo': comp.get('versionInfo'),
        'supplier': comp.get('supplier'),
        'pkgtype': pkgtype
    })
