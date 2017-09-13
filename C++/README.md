COMPILING ICAROUS
-

To compile the ICAROUS software in a Unix environment, type

```
$ make
```

###LAUNCHING ICAROUS WITH PLEXIL

- Compile the plexil plan to create a .plx file. Refer to plexil-4/examples/basic for more details.
- Launch the plexil executive (use the provided run.sh script as follows)

```
	$./run.sh PLAN
```

- Launch ICAROUS

```
	$./run.sh ICPL params/icarous.txt
```


