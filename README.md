# VEK language

**Vek Language** _is a fast, bytecode-compiled language with a custom VM_ (just like java with jvm)

**Code that shows capabilities (run with `./main {filename}.vek`)**

```vek
// functions that have params and return types
func calculate_bonus(int score) int {
    let int bonus = 0;

    // Basic control flow
    if (score > 80) {
        bonus = 100;
    } else if (score > 50) {
        bonus = 50;
    } else {
        bonus = 10;
    }

    ret bonus;
}

func main() int {
    // Variables (void, bool, str, char, int/int64, int32, float/float32, float64)
    let str title = "VekLang"; // Type is NECESSARY.
    let bool is_awesome = true;
    
    sys::out(title);
    sys::out("Is VekLang good?");
    sys::out(is_awesome);

    // Dynamic arrays
    let int scores[] = {90, 45, 85}; // Also `let int scores[3] = {90, 45, 85}` - COMPLETELY SAME!!
    sys::out("Original Array:");
    sys::out(scores);

    // Array methods (push and pop)
    scores.push(100);
    let int popped = scores.pop(); 
    
    sys::out("We pushed 100 and popped it. The popped value is:");
    sys::out(popped);

    // Array indexing
    let int first_score = scores[0];
    sys::out("The first score is:");
    sys::out(first_score);

    // Custom function using array data
    let int my_bonus = calculate_bonus(first_score);
    sys::out("Bonus points earned:");
    sys::out(my_bonus);

    sys::out("EOF");

    ret 0;
}
```

## Syntax:
```vek
// This file won't run. it's supposed to be a syntax orientation.
load mainstream; // works just like #include or import

class Test {
    private {
        let str name;
        let str surname;
    }

    public {
        name = "John";
        surname = "Doe";

        sys::out(name + " " + surname);   
    }
}

func main() int {
    let int num = 0 + 5;
    num = 100;
    let str string = "hi";

    # Print + Return integer
    sys::out(string);
    ret num;
}
```

## TODO
- [ ] Write a proper README.md 
- [ ] Organize src .cpp files instead of using .hpp only
- [ ] Add classes
- [ ] Organize the code
- [ ] CMake file
- [ ] Compiler and VM supposed to be different things. (_vekvm {filename}.vek_ to run, _vek run/build_ - developer tool) 

## Acknowledgements
This project was built using AI tools as an educational aid and coding assistant. 
All architectural decisions, VM logic, and final debugging were done by the author.