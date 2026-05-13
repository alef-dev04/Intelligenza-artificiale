package ShursLemma.SatToLemma;

import it.uniroma1.di.tmancini.utils.*;
import it.uniroma1.di.tmancini.teaching.ai.SATCodec.*;
import java.util.*;
import java.io.IOException;

public class satToLemma {
    public static void main(String[] args) throws IOException {
        SATModelDecoder decoder = new SATModelDecoder(args);
        decoder.run();
        int num_vars = decoder.getMaxVar();
        ArrayList<Integer> output_urna1 = new ArrayList<>();
        ArrayList<Integer> output_urna2 = new ArrayList<>();
        ArrayList<Integer> output_urna3 = new ArrayList<>();

        for (int i = 1; i <= num_vars; i++) {
            Boolean val = decoder.getModelValue(i);

            SATModelDecoder.Var variable = decoder.decodeVariable(i);

            // Saltiamo le variabili ausiliarie create dal solver o non decodificabili
            if (variable == null) {
                continue;
            }

            String family = variable.getFamily();
            List<Integer> indices = variable.getIndices();

            // Se la variabile appartiene alla famiglia "X" (definita in lemmaToSat) ed è
            // vera (true)
            if (val != null && val && "X".equals(family)) {
                int biglia = indices.get(0);
                int urna = indices.get(1);
                if (urna == 1) {
                    output_urna1.add(biglia);
                } else if (urna == 2) {
                    output_urna2.add(biglia);
                } else if (urna == 3) {
                    output_urna3.add(biglia);
                }
            }
        }
        System.out.println("Urna 1: " + output_urna1);
        System.out.println("Urna 2: " + output_urna2);
        System.out.println("Urna 3: " + output_urna3);
    }
}
