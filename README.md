# Konwerter_eLeReS_FrSky
Konwerter telemetrii eLeReSa dla TG9XR

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
