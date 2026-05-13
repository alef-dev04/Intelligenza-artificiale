package ShursLemma.LemmaToSat;

import it.uniroma1.di.tmancini.utils.*;
import it.uniroma1.di.tmancini.teaching.ai.SATCodec.*;
import java.util.*;

public class lemmaToSat {
    public static void main(String[] args) {
        // numero delle biglie
        int n = 5;
        IntRange num_biglie = new IntRange("num_biglie", 1, n);
        IntRange num_urne = new IntRange("num_urne", 1, 3);

        SATEncoder encoder = new SATEncoder("Shurs_lemma", "Shurs_lemma_enc.cnf");
        encoder.defineFamilyOfVariables("X", num_biglie, num_urne);

        //encoder.enableDebugMode();

        RangeProduct coppia_urne = new RangeProduct("coppia_urne", num_urne, 2);
        

        Iterator<List<Integer>> coppia_urne_iterator;

        //una biglia deve stare in almeno un'urna
        for(int i: num_biglie.values()){
            for(int j: num_urne.values()){
                encoder.addToClause("X", i, j);
            }
            encoder.endClause();
        }

        //una biglia deve stare in massimo un'urna
        //per ogni biglia
        for (int i : num_biglie.values()) {
            coppia_urne_iterator = coppia_urne.iterator(RangeProduct.FILTER.ALLDIFF_ORDERED);
            //vado a dire che una biglia non può stare in due urne conteporaneamente
            //uso l'iteratore poiché mi restituisce le coppie di urne come
            //(1,2)(1,3)(2,3) che sono esattamente quelle che mi servono
            while (coppia_urne_iterator.hasNext()) {
                List<Integer> coppia = coppia_urne_iterator.next();
                encoder.addNegToClause("X", i, coppia.get(0));
                encoder.addNegToClause("X", i, coppia.get(1));
                encoder.endClause();
            }
        }
        
        RangeProduct terne_biglie = new RangeProduct("terne_biglie", num_biglie, 3);
        //uso un iteratore delle terne di biglie in modo da controllare quando i valori delle biglie
        //devono essere sottoposte a vincoli
        Iterator<List<Integer>> terne_biglie_iterator = terne_biglie.iterator(RangeProduct.FILTER.ALLDIFF_ORDERED);
        while(terne_biglie_iterator.hasNext()){
            
            List<Integer> terna = terne_biglie_iterator.next();
            
            int num_1 = terna.get(0);
            
            int num_2 = terna.get(1);
            
            int num_3 = terna.get(2);
            
            if(num_1+num_2==num_3){
                for(int j: num_urne.values()){
                    encoder.addNegToClause("X", num_1, j);
                    encoder.addNegToClause("X", num_2, j);
                    encoder.addNegToClause("X", num_3, j);
                    encoder.endClause();
                }

                
            }
                
        }
        encoder.end();


    }
}
