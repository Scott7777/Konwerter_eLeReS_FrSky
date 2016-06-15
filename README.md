# Konwerter_eLeReS_FrSky
Konwerter telemetrii eLeReSa dla TG9XR

15.06.2016
- Poprawka wysy³ania telemetrii do aparatury. Teraz powinno dzia³aæ te¿ dla  Esky9x - 9XR Pro.

15.03.2016

- Zerowanie parametrów po zaniku telemetrii. Nie zeruje siê pozycja z GPSa.
- Obliczanie ciœnienia i wysokoœci barometrycznej npm.

14.03.2016

- Modyfikacja prêdkoœci portu debug. Treaz jes 57600 baud. Na 19200 nie wyrabia³em siê z wysy³aniem FrSky.
- Korekta timingów. 


13.03.2016

- Poprawka wysy³ania RSSI i RCQ.
- Dodanie symulacji komunikatów z eLeReSa
  Wysy³aj¹c "TEST:lista parametrów" do portu DEBUG symulujemy wys³anie tych danych z eLeReSa.
  np. TEST:RSSI=091 RCQ=075 U=12.2V I=00.7A h=0500. Wys³¹nie TEST:STOP koñczy symulacjê.
- Zmiana timingów wysy³ania ramek. Ju¿ nie znika TSSI(RCQ). Dzia³a te¿ lepiej zmienna TmOK mówi¹ca o poprawnym odbiorze telemetrii w aparaturze.


12.03.2016

Jeœli dany paramert z telemetrii nie jest odbierany przez 5 sekund - zerujê go i przestajê wysy³aæ dalej.


11.03.2016
	
Doda³em sprawdzanie d³ugoœci stringu dla danych z eLeReSa. Odfiltrowuje to wiêkszoœæ b³êdów w transmisji.


10.03.2016

Pierwsza Beta.
Pod³¹czone do eLeReSa i sprawdzone.
Dodany szczegó³owy debug.

Dzia³a - mo¿na testowaæ.


07.03.2016

Dzia³a czytanie z eLeReSa:
- RSSI
- RCQ
- napiêcie w modelu
- pr¹d w modelu
- temperatura w modelu
- napiêcie odbiornika
- temperatura odbiornika
- ciœnienie - narazie tylko jako wartoœæ liczbowa
- tryb lotu jako liczba
- Hdg
- Fix gpsu - liczba
- iloœæ satelit
- kurs modelu
- prêdkoœæ modelu z GPS
- wysokoœæ modelu z GPS

Wysy³anie telemetrii:
- temperatura w modelu T1
- temperatura nadajnika T2
- napiêcie w modelu A1
- pr¹d w modelu A2
- RSSI
- RCQ
- wysokosc GPS
- prêdkoœæ GPS
- kurs
- pozycja GPS
- wskaŸnik FUEL jako napiêcie baterii modelu

