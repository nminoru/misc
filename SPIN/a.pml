mtype = {F, S};
mtype turn = F;

active proctype Producer()
{
    do 
      :: (turn == F) -> printf("Producer\n"); turn = S;
    od
}

active proctype Consumer()
{
    do 
      :: (turn == S) -> printf("Consumer\n"); turn = F;
    od
}
