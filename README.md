# Konwerter_eLeReS_FrSky
Konwerter telemetrii eLeReSa dla TG9XR

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
