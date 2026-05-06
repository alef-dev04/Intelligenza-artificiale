package satToQueens;

import it.uniroma1.di.tmancini.utils.*;
import it.uniroma1.di.tmancini.teaching.ai.SATCodec.*;
import java.util.*;
import java.io.IOException;
import java.lang.Math;

public class satToQueens {
    public static void main(String[] args) throws IOException {
        SATModelDecoder decoder = new SATModelDecoder(args);
        decoder.run();
        int num_vars = decoder.getMaxVar();
        

        for(int i=0; i< num_vars; i++){
            Boolean val = decoder.getModelValue(i);

            SATModelDecoder.Var variable = decoder.decodeVariable(i);
            String family = variable.getFamily();
            List<Integer> indices = variable.getIndices();
            
        }
    }
}
