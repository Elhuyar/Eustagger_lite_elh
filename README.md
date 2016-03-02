# Eustagger_Elh
Elhuyar version of eustagger_lite lemmatizer/tagger for Basque, with speed performance improvements and server functionality added.

You can find the original version developed by the IXA NLP group of the University of the Basque Contry at:

https://github.com/ixa-ehu/eustagger


Differences of Eustagger_elh with the original version:

- Eustagger_elh is faster because some improvements made in the morphosyntactic analysis step. Due to those changes it has a higher memory usage.
- Eustagger_elh has the option of running in server mode using -p parameter, an example client is provided: eustagger_client.pl 
- Eustagger_elh only has conll output format. Because it was forked from an older version, there is no NAF output. 


How to install Eustagger_elh:

Follow the instructions in IRAKURRI (Basque) or INSTRUCCIONES (Spanish) files.

How to use Eustagger_elh:

Upon installation run eustagger_lite executable:

'''
eustagger_lite proba.txt > proba.txt.tagged
'''


Contact: 

Xabier Saralegi and Iñaki San Vicente
Elhuyar Foundation
{x.saralegi,i.sanvicente}@elhuyar.com
