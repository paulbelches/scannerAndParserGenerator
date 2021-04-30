# Scanner and Parser Generator

## fileReader.cpp
### Purpose:
      Using a input atg file, generate a token scanner for the input gramatic. 
 
### Input:
      Atg file with a Coco/R grammatic.  
 
### Output:
      The compiled code for a scanner that can identify tokens from the grammar. 
      
### Compile:  
     c++ fileReader.cpp -o main.run
  
### Usage:
     ./main.run <filepath>
 
### Execute the scanner:
     ./scanner.run <filepath to be analized>


## fileGenerator.py
### Purpose:
      Generate files for the example gramatics.   
 
### Output:
      3 multi token files:
            hex.txt
            ari.txt
            double

### Usage:
     python3 fileGenerator.py
