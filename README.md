# the plan is to build a chip-8 emulator
Running `make run` should do the trick for now
## Appunti
Se ho capito bene come funziona tutta la faccenda non abbiamo nessun interrupt da simulare (devo controllare) 
ma semplicemente per i comandi c'è una "sysxall" che blocca fino a che non viene premuto qualcosa 
probabilmente se sdl me lo permetto conto di usare un timer a 60hz come stepper e in caso di blocco per attesa input direi 
che basti decrementare il counter (o non aumentarlo) e aspettare per un evento diverso, oppure fare un secondo poll di eventi(no)
L'idea per l'input direi potrebbe essere una ispirata a imgui (credo)
cioè semplicemente a inizio loop prendo lo stato della tastiera (sono solo 16 tasti) 
e lo sparo in un mio array dove 1 = premuto e 0 non premuto magari anche con 2 = rilasciato in questo ciclo
Oppure boh, ci penserò con calma più avanti


## TODO
- [x] Trovare una test suite che sia valida a sufficienza per i nostri biechi scopi e che utilizzi le istruzioni "classiche" (alla fine ne ho trovata una con quelle nuove ma vbb)
- [ ] Sarebbe interessante andare a vedere come si fanno i test in c visto che effettivamente non mi è mai capitato di farne ehehe
- [ ] Un sistema di logging anche non sarebbe terribile, potrebbe anche essere solo selezionato a compile time con degli #ifdef 
- [ ] Sarebbe poi carino tipo un coso per analizzare la cpu e magari fare anche step by step
- [ ] Per adesso lo stepper non permette di fornire input
