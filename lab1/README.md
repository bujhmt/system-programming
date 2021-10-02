CLI command to generate input text text: 

```bash 
base64 /dev/urandom | head -c 10000000 > input.txt
```

Run lab1-1.c:
```bash
gcc lab1-1.c -O3 && ./a.out input.txt output.txt   
```

Run lab1-2.c:
```bash
gcc lab1-2.c && ./a.out key
```
