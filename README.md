# Konwerter_eLeReS_FrSky
Konwerter telemetrii eLeReSa dla TG9XR

15.06.2016
- Poprawka wysy�ania telemetrii do aparatury. Teraz powinno dzia�a� te� dla  Esky9x - 9XR Pro.

15.03.2016

- Zerowanie parametr�w po zaniku telemetrii. Nie zeruje si� pozycja z GPSa.
- Obliczanie ci�nienia i wysoko�ci barometrycznej npm.

14.03.2016

- Modyfikacja pr�dko�ci portu debug. Treaz jes 57600 baud. Na 19200 nie wyrabia�em si� z wysy�aniem FrSky.
- Korekta timing�w. 


13.03.2016

- Poprawka wysy�ania RSSI i RCQ.
- Dodanie symulacji komunikat�w z eLeReSa
  Wysy�aj�c "TEST:lista parametr�w" do portu DEBUG symulujemy wys�anie tych danych z eLeReSa.
  np. TEST:RSSI=091 RCQ=075 U=12.2V I=00.7A h=0500. Wys��nie TEST:STOP ko�czy symulacj�.
- Zmiana timing�w wysy�ania ramek. Ju� nie znika TSSI(RCQ). Dzia�a te� lepiej zmienna TmOK m�wi�ca o poprawnym odbiorze telemetrii w aparaturze.


12.03.2016

Je�li dany paramert z telemetrii nie jest odbierany przez 5 sekund - zeruj� go i przestaj� wysy�a� dalej.


11.03.2016
	
Doda�em sprawdzanie d�ugo�ci stringu dla danych z eLeReSa. Odfiltrowuje to wi�kszo�� b��d�w w transmisji.


10.03.2016

Pierwsza Beta.
Pod��czone do eLeReSa i sprawdzone.
Dodany szczeg�owy debug.

Dzia�a - mo�na testowa�.


07.03.2016

Dzia�a czytanie z eLeReSa:
- RSSI
- RCQ
- napi�cie w modelu
- pr�d w modelu
- temperatura w modelu
- napi�cie odbiornika
- temperatura odbiornika
- ci�nienie - narazie tylko jako warto�� liczbowa
- tryb lotu jako liczba
- Hdg
- Fix gpsu - liczba
- ilo�� satelit
- kurs modelu
- pr�dko�� modelu z GPS
- wysoko�� modelu z GPS

Wysy�anie telemetrii:
- temperatura w modelu T1
- temperatura nadajnika T2
- napi�cie w modelu A1
- pr�d w modelu A2
- RSSI
- RCQ
- wysokosc GPS
- pr�dko�� GPS
- kurs
- pozycja GPS
- wska�nik FUEL jako napi�cie baterii modelu

