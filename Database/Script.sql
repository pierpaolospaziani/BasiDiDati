use `noleggio-film`;

GRANT EXECUTE ON procedure `noleggio-film`.`login` TO 'login';

call login('SPZ','Pier',@a);
call login('PPL','Pier',@a);
call login('S','Pier',@a);
Select @a;

call report_mensile('SPZ','2','2020',@ore,@esiste_impiegato);
Select @ore;
Select @esiste_impiegato;
call report_annuale('SPZ','2020',@ore,@esiste_impiegato);
Select @ore;
Select @esiste_impiegato;

call aggiungi_turno('2021-03-04','2021-03-24','Commesso',1,'SPZ');

call aggiungi_centro('PPL','Via Mia','1','Frosinone');
call cambia_responsabile('1','PPL');

call turni_del_giorno('2020-2-3',1);

call aggiungi_copia('IT','Andrés Muschietti','1','1','2');

call aggiungi_film('IT_2','Andrés Muschietti','2000');

call mostra_film_nuovi;

call noleggi_scaduti('1');

call film_disponibili_noleggio('1');

call noleggia('SPZPPL','2021-09-09','1','IT','Andrés Muschietti',@c_noleggio,@c_giornaliero);
Select @c_noleggio;
Select @c_giornaliero;

call restituisci_film('1',@costo_extra);
Select @costo_extra;

select * from utenti;
select * from Impiegato;
select * from Turno order by `Data_Inizio` asc;
select * from Centro;
select * from Settore;
select * from Copia_Film order by `Titolo_Copia`, `Regista_Copia` asc;
select * from Film;
select * from Remake;
select * from Recita;
select * from Attore;
select * from Noleggio;
select * from Cliente;
select * from Indirizzo;
select * from Email;
select * from Telefono;
select * from Cellulare;















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