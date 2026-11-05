# VEK language

**Vek Language** _is a fast, bytecode-compiled language with a custom VM_ (just like java with jvm)
  
**Language Features:**
- [x] Variables & Math
- [x] If/Else Control Flow
- [x] Functions & Scopes
- [x] Dynamic Arrays (`push`, `pop`)
- [ ] `while` and `for` loops
- [ ] Structs / Classes
- [ ] String manipulation methods (`.length()`, `.split()`)
  
**Code that shows capabilities (run with `./main {filename}.vek`)**
```
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

## TODO
- [ ] Organize src .cpp files instead of using .hpp only.
- [ ] Add classes.
- [ ] Implement while/for loops.
- [ ] Organize the code.
- [ ] CMake file.
- [ ] Compiler and VM supposed to be different things. (_vekvm {filename}.vekbc_ to run, _vek run/build_ - developer tool) (turn .vekbc is a bytecode file, with magic, 3 bytes in header.)

## Acknowledgements
This project was built using AI tools as an occasional educational aid and a minor coding assistant (primarily for boilerplate and formatting). 
The overall architecture, core compiler/VM design, implementation, and final debugging were entirely driven and executed by the author.