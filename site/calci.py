import cgi
form = cgi.FieldStorage() 

input1 = form.getvalue('input1')
input2  = form.getvalue('input2')

result = int(input1)+int(input2)
html = open("calculator.html".read())
header = "Content-Type: text/html; charset=UTF-8\n\n"

output = header + html.format(result=result)
print (output)