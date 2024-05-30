#!/usr/bin/env node

console.log("Content-type: text/html\n");
console.log("<html>");
console.log("<head>");
console.log("<title>Current Time</title>");
console.log("</head>");
console.log("<body>");
console.log("<h1>Current Time:</h1>");
console.log("<p>" + new Date().toLocaleString() + "</p>");
console.log("</body>");
console.log("</html>");
