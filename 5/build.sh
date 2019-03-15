gcc server.c -o server
gcc client.c -o client

read -p 'No of radom numbers to be generated: ' n
counter=1
while [ $counter -le $n ]
do
echo $(( ( RANDOM % 10000 )  + 1 ))
((counter++))
done