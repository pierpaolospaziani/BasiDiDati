use mydb;

call login('Pierpy',MD5('Pier'),@a);
Select @a;

call report_mensile_ore('SPZ','02','2020');

select * from utenti;
select * from Impiegato;
select * from Centro;
select * from Turno;




delete from utenti where username = 'Pierpy';
insert into utenti values('Pierpy',MD5('Pier'),'Impiegato');
insert into Telefono(Numero_Telefono) values('346');
insert into Impiegato values('SPZ','Pierpy','Pierpaolo','Liceo','346');
insert into Centro(CF_Responsabile) values('SPZ');
insert into Turno(Carica,Data_Inizio,Data_Fine,Codice_Centro,CF_Impiegato) values('Cassiere','2020-02-02','2020-02-05','1','SPZ');
insert into Turno(Carica,Data_Inizio,Data_Fine,Codice_Centro,CF_Impiegato) values('Cassiere','2020-01-04','2020-01-07','1','SPZ');
insert into Turno(Carica,Data_Inizio,Data_Fine,Codice_Centro,CF_Impiegato) values('Cassiere','2020-02-25','2020-03-05','1','SPZ');
insert into Turno(Carica,Data_Inizio,Data_Fine,Codice_Centro,CF_Impiegato) values('Cassiere','2020-01-29','2020-02-01','1','SPZ');
insert into Turno(Carica,Data_Inizio,Data_Fine,Codice_Centro,CF_Impiegato) values('Cassiere','2020-04-06','2020-04-25','1','SPZ');









