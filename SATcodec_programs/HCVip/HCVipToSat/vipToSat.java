package HCVip.HCVipToSat;

import it.uniroma1.di.tmancini.utils.*;
import it.uniroma1.di.tmancini.teaching.ai.SATCodec.*;

import java.beans.Encoder;
import java.util.*;

import HCVip.Coppia;

public class vipToSat {
    public static void main(String[] args) {
        //insieme I degli indirizzi
        ArrayList<String> ind = new ArrayList<>();

        //insieme V dei vip
        ArrayList<String> vip = new ArrayList<>();

        //insieme bus
        ArrayList<Coppia> bus = new ArrayList<>();


        // 1. Popoliamo l'insieme I degli indirizzi
        ind.addAll(Arrays.asList("casa", "A", "B", "C", "D"));

        // 2. Popoliamo l'insieme V dei VIP
        vip.add("B");

        // 3. Popoliamo l'insieme bus (il grafo diretto)
        // Archi per il percorso corretto
        bus.add(new Coppia("casa", "A"));
        bus.add(new Coppia("A", "B"));
        bus.add(new Coppia("B", "C"));
        bus.add(new Coppia("C", "D"));
        bus.add(new Coppia("D", "casa"));

        // Archi per un percorso "trappola" (ciclo Hamiltoniano, ma VIP nella seconda metà)
        bus.add(new Coppia("casa", "C"));
        bus.add(new Coppia("C", "A"));
        bus.add(new Coppia("A", "D"));
        bus.add(new Coppia("D", "B"));
        bus.add(new Coppia("B", "casa"));

        // Qualche arco extra (vicoli ciechi o percorsi non Hamiltoniani) per confondere il solver
        bus.add(new Coppia("B", "A"));
        bus.add(new Coppia("C", "casa"));
        bus.add(new Coppia("casa", "D"));

        IntRange num_ind = new IntRange("num_ind", 0, ind.size());
        IntRange num_vip = new IntRange("num_vip", 0, vip.size());
        IntRange num_bus = new IntRange("num_bus", 0, bus.size());
        //la variabile per mantenere il tempo massimo raggiungibile
        //è uguale al numero di indirizzi, dato che in ogni istante dobbiamo visitare un indirizzo diverso
        //e non possiamo visitare un indirizzo due volte
        IntRange T = new IntRange("tempo_max", 0, num_ind.size());

        RangeProduct ind_couple = new RangeProduct("ind_couple", num_ind, 2);
        RangeProduct couple_time = new RangeProduct("couple_time", T, 2);

        SATEncoder encoder = new SATEncoder("HCVip", "HCVip_enc.cnf");
        //dove in_x_t denoterà che al tempo t ero nel posto x
        encoder.defineFamilyOfVariables("in", num_ind, T);

        Iterator<List<Integer>> ind_iterator;
        Iterator<List<Integer>> time_iterator;

        //in ogni istante di tempo devo essere in almeno una posizione
        for(int t: T.values()){
            ind_iterator = ind_couple.iterator(RangeProduct.FILTER.ALLDIFF_ORDERED);
            while(ind_iterator.hasNext()){
                List<Integer> couple = ind_iterator.next();
                encoder.addToClause("in", couple.get(0), t);
                encoder.addToClause("in", couple.get(1), t);
                encoder.endClause();
            }
        }

        //in ogni istante di tempo devo essere in al massimo una posizione
        for(int t: T.values()){
            ind_iterator = ind_couple.iterator(RangeProduct.FILTER.ALLDIFF_ORDERED);
            while(ind_iterator.hasNext()){
                List<Integer> couple = ind_iterator.next();
                encoder.addNegToClause("in", couple.get(0), t);
                encoder.addNegToClause("in", couple.get(1), t);
                encoder.endClause();
            }
        }


        //posso stare a casa solo nell'istante di tempo iniziale e finale
        //assumo che il primo elemento dell'insieme degli indirizzi è casa
        for(int t: T.values()){
            if(t==0 || t==T.getMax()){
                encoder.addToClause("in", 0, t);
                encoder.endClause();
            }else{
                encoder.addNegToClause("in", 0, t);
                encoder.endClause();
            }
        }

        // non posso stare nel posto x al tempo t e nel posto y al tempo t+1
        // se x e y non sono collegate da un bus
        ind_iterator = ind_couple.iterator(RangeProduct.FILTER.ALLDIFF_ORDERED);
        while(ind_iterator.hasNext()){
            List<Integer> couple = ind_iterator.next();
            if (!bus.contains(couple)){
                for(int t=0; t<T.getMax()-1; t++){
                    encoder.addNegToClause("in", couple.get(0), t);
                    encoder.addNegToClause("in", couple.get(1), t+1);
                    encoder.endClause();
                }
            }
        }

        // non posso visitare uno stesso posto due volte
        time_iterator = couple_time.iterator(RangeProduct.FILTER.ALLDIFF_ORDERED);
        while(time_iterator.hasNext()){
            List<Integer> couple = time_iterator.next();
            for(int x: num_ind.values()){
                encoder.addNegToClause("in", x, couple.get(0));
                encoder.addNegToClause("in", x, couple.get(1));
                encoder.endClause();
            }
        }

        // i vip devono essere visitati per primi
        // quindi assegnamo a false tutte le possibili visite ai vip nella "seconda metà"
        for(int t=num_vip.getMax()+1; t<T.getMax(); t++){
            for(int x: num_vip.values()){
                encoder.addNegToClause("in", x, t);
                encoder.endClause();
            }
        }

        encoder.end();
    }
}
