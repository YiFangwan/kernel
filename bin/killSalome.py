#!/usr/bin/env python
import os, string

from killSalomeWithPort import killSalome

user = os.getenv('USER')
for file in os.listdir("/tmp"):
    l = string.split(file, "_")
    if len(l) >= 4:
        if file[:len(user)] == user:
            if l[len(l)-2] == "SALOME" and l[len(l)-1] == "pidict":
                killSalome(l[len(l)-3])
    pass
