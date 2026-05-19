package HCVip.SatToHCVip;

import it.uniroma1.di.tmancini.utils.*;
import it.uniroma1.di.tmancini.teaching.ai.SATCodec.*;
import java.util.*;
import java.io.IOException;

public class satToHCVip {
    public static void main(String[] args) throws IOException {
        SATModelDecoder decoder = new SATModelDecoder(args);
        decoder.run();
        int num_vars = decoder.getMaxVar();

        // mappa degli indirizzi usata in vipToSat
        String[] ind = { "casa", "A", "B", "C", "D" };

        // vettore per salvare le posizioni in ordine di tempo
        String[] percorso = new String[6];

        for (int i = 1; i <= num_vars; i++) {
            Boolean val = decoder.getModelValue(i);
            SATModelDecoder.Var variable = decoder.decodeVariable(i);

            if (variable == null) {
                continue;
            }

            String family = variable.getFamily();
            List<Integer> indices = variable.getIndices();

            // se la variabile "in" è vera
            if (val != null && val && "in".equals(family)) {
                int indirizzo = indices.get(0);
                int tempo = indices.get(1);

                // salviamo l'indirizzo all'istante di tempo corrispondente
                percorso[tempo] = ind[indirizzo];
            }
        }

        // stampiamo il percorso ordinato
        System.out.println("====== PERCORSO DEL BUS ======");
        for (int t = 0; t < percorso.length; t++) {
            if (t > 0) {
                System.out.print(" -> ");
            }
            System.out.print(percorso[t] + " (t=" + t + ")");
        }
        System.out.println("\n==============================");
    }
}
