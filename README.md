# M3OEP-nlippiat

Your name(s)
- Any added installations necessary to run the program
- Custom JSON LIbrary for C++: (installation is handled in my CMakeList)
  https://github.com/nlohmann/json
- Pandas for Python
- A summary of your program
- This program is a task/project manager, you can create projects and add tasks inside the projects. Then the new feature is an analysis report, the data is written to a JSON and then uses python pandas to analyze your tasks and reccomend you your next three tasks
- Which language your program starts in, and which other language(s) it uses
- C++ and Python
- How you use each language in the program and why the language is a good choice for that use
- I use C++ for the foundation because it is fast and effecient, with strong oop typing. I use python and the pandas library because python is very strong for light weight data analysis. 
- How the languages are connected (i.e. which one calls which, and where in your code)
- It starts in C++, sends data to python via a JSON file, python outputs JSON and the results are given to the user in C++
- Any known bugs at time of submission
- None Known
- Future work (how you could expand the program with more time)
- If i had more time I wouldve liked to have used Python to also create graphs based on the analysis
- Citations for any code not written by you or the instructor
- No code not specifically written by me, but I relied on Documentation for a Custom JSON library and for Pandas usage.
- Pandas: https://www.w3schools.com/python/pandas/default.asp
- JSON:  https://github.com/nlohmann/json https://json.nlohmann.me/
- 
- M2OEP Citations: Move operator: projects.push_back(std::move(p)) : https://www.geeksforgeeks.org/cpp/move-assignment-operator-in-cpp-11/ File system library in c++:https://www.geeksforgeeks.org/cpp/file-system-library-in-cpp-17/ Clone() std::unique_ptr TimedTask::clone() const :https://www.fluentcpp.com/2017/09/08/make-polymorphic-copy-modern-cpp/

M1OEP Citations:

std::cin.clear(); std::cin.ignore(10000, '\n'); This is for clearing and handling invalid inputs: https://www.geeksforgeeks.org/cpp/cin-clear-function-in-cpp/ https://www.geeksforgeeks.org/cpp/cin-ignore-function-in-cpp/
Deleting from a vector: https://www.geeksforgeeks.org/cpp/vector-erase-in-cpp-stl/ tasks.erase(this->tasks.begin() + index);
- The grade you think you have earned, based on the grading rubric below, with justification
Based on the grading rubric I think I have earned an 85 because I reasonably incorporate another language but not multiple. I also did do a deep dive into a specific library which could justify bonus points. I also reasonably transition the data between langauges and I think my langauge selection makes sense. However my code and documentation is rather sloppy.






