use `noleggio-film`;

GRANT EXECUTE ON procedure `noleggio-film`.`login` TO 'login';
GRANT EXECUTE ON procedure `noleggio-film`.`aggiungi_turno` TO 'manager';
GRANT EXECUTE ON procedure `noleggio-film`.`turni_del_giorno` TO 'manager';
GRANT EXECUTE ON procedure `noleggio-film`.`report_mensile` TO 'manager';
GRANT EXECUTE ON procedure `noleggio-film`.`report_annuale` TO 'manager';
GRANT EXECUTE ON procedure `noleggio-film`.`aggiungi_impiegato` TO 'manager';
GRANT EXECUTE ON procedure `noleggio-film`.`cambia_responsabile` TO 'manager';
GRANT EXECUTE ON procedure `noleggio-film`.`aggiungi_film` TO 'manager';
GRANT EXECUTE ON procedure `noleggio-film`.`aggiungi_copia` TO 'manager';
GRANT EXECUTE ON procedure `noleggio-film`.`dequalifica_film` TO 'manager';
GRANT EXECUTE ON procedure `noleggio-film`.`film_disponibili_noleggio` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`noleggia` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`restituisci_film` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`noleggi_scaduti` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`noleggi_cliente` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`info_cliente` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`aggiungi_cliente` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`aggiungi_cellulare` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`aggiungi_telefono` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`aggiungi_indirizzo` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`aggiungi_email` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`elimina_cellulare` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`elimina_telefono` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`elimina_indirizzo` TO 'impiegato';
GRANT EXECUTE ON procedure `noleggio-film`.`elimina_email` TO 'impiegato';

call login('SPZ','Pier',@a, @nome);
call login('PPL','Pier',@a, @nome);
call login('S','Pier',@a, @nome);
Select @a;
Select @nome;

call report_mensile('SPZ','2','2022',@ore);
Select @ore;
call report_annuale('SPZ','2022',@ore);
Select @ore;

call aggiungi_turno('2022-02-04','2022-02-4','Cashier',1,'AAA');

call aggiungi_turno('2022-02-03','2022-02-3','Cashier',1,'AAA');
call aggiungi_turno('2022-02-03','2022-02-3','Cashier',1,'BBB');
call aggiungi_turno('2022-02-03','2022-02-3','Cashier',1,'CCC');
call aggiungi_turno('2022-02-03','2022-02-3','Cashier',1,'DDD');

call aggiungi_impiegato('1232','P','P','1','Impiegato');

call aggiungi_centro('PPL','Via Mia','1','Frosinone');
call cambia_responsabile('1','Spaziani');

call turni_del_giorno('2022-2-3',1);

call aggiungi_copia('IT','Andrés Muschietti','1','1','2');

call aggiungi_film('IT_2','Andrés Muschietti','2000');

call mostra_film_nuovi;

call noleggi_scaduti('1');

call film_disponibili_noleggio('1');

call noleggia('SPZPPL','2021-08-1','1','IT','Andrés Muschietti',@days,@c_noleggio,@c_giornaliero);
Select @days;
Select @c_noleggio;
Select @c_giornaliero;

call restituisci_film('1',@costo_extra);
Select @costo_extra;

select count(Impiegato_Turno) from `Turno` where `Impiegato_Turno` = 'SPZ' and `Data_Inizio` <= '2022-01-05' and `Data_Fine` >= '2022-01-05';

call noleggi_cliente('SPZPPL');

call info_cliente('SPZPPL');

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
select * from Noleggio order by `Data_Restituzione` asc;
select * from Cliente;
select * from Indirizzo;
select * from Email;
select * from Telefono;
select * from Cellulare;

call aggiungi_cellulare('000','SPZPPL');

















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