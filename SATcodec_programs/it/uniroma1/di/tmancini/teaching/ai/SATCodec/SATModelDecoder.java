package it.uniroma1.di.tmancini.teaching.ai.SATCodec;

import java.util.*;
import java.io.*;
import it.uniroma1.di.tmancini.utils.*;

public class SATModelDecoder {

	private CmdLineOptions clo;
	
	public static class Var {
		private final String family;
		private final List<Integer> indices;
		
		private Var(String v) {
			int i = v.indexOf("[");
			family = v.substring(0, i);
			String[] indArr = v.substring(i+1, v.indexOf("]")).split("\\s*,\\s*");
			this.indices = new ArrayList<Integer>(indArr.length);
			for(String s : indArr) {
				this.indices.add( Integer.parseInt(s) );
			}
		}
		public String toString() {
			return family + indices;
		}
		public String getFamily() {
			return family;
		}
		public List<Integer> getIndices() {
			return Collections.unmodifiableList(indices);
		}
	}
	
	// The max used var is the last index of the dictionary (==size-1). dictionary[0] is NOT used.
	private List<Var> dictionary;
	
	private List<Boolean> model;
	private String args[];
	List<String> toHide = new ArrayList<String>();
	
	public SATModelDecoder(String args[]) {	
		clo = new CmdLineOptions("SATModelDecoder", "2013-08-26", "Toni Mancini (http://tmancini.di.uniroma1.it)", "This program decodes the model returned by a SAT solver using the dictionary given as a comment in the DIMACS file.\nSupported SAT solvers are minisat and zchaff.");
		clo.addOption("cnf", "The DIMACS file given as input to the SAT solver and containing the dictionary", true);
		clo.addOption("solver", "The name of the SAT solver: {minisat, zchaff, picosat}", true);
		clo.addOption("model", "The output file produced the SAT solver", true);
		clo.addOption("hide", "Variable families to hide, as a single family name or a double-quotes-separated list: \"fam1 fam2 ... famn\"");
		
		this.args = args;
	}
	
	public void run() throws java.io.IOException {
		clo.parse(args);
		
		String cnf = clo.getOptionValue("cnf");
		String solver = clo.getOptionValue("solver");
		String satout = clo.getOptionValue("model");
		
		
		if (clo.getOptionValue("hide") != null) {			
			toHide.addAll( Arrays.asList(clo.getOptionValue("hide").split(" ")) );
			//System.out.println("DEBUG: Hiding variable families: " + toHide);
		}
		
		dictionary = parseDictionary(cnf);
		
		this.model = null;
		
		if (solver.equalsIgnoreCase("zchaff")) {
			this.model = zChaff_parseModel(dictionary.size(), satout);
		} else if (solver.equalsIgnoreCase("minisat")) {
			this.model = minisat_parseModel(dictionary.size(), satout);
		} else if (solver.equalsIgnoreCase("picosat")) {
			this.model = picosat_parseModel(dictionary.size(), satout);
		} else {
			throw new Error("Solver '" + solver + "' unknown. Supported solvers are minisat and zchaff");
		}		
	}

	
	public String getOptionValue(String o) {
		return clo.getOptionValue(o);
	}
	
	public void setOptionValue(String o, String v) {
		clo.setOptionValue(o,v);
	}

	
	public boolean isFlagSet(String f) {
		return clo.isFlagSet(f);
	}
	public void setFlag(String f) {
		clo.setFlag(f);
	}	

	public boolean isSAT() { 
		return model != null;
	}
	
	public int getMaxVar() {
		if (!isSAT()) return 0;
		return dictionary.size()-1;
	}
	
	/** Returns the value of variable v in the model. NULL means 'any value' (SAT solvers may return partial models) */
	public Boolean getModelValue(int v) {
		if (!isSAT() || v < 1 || v >= model.size()) throw new Error("getModelValue(v): error in value for v: " + v)	;
		return model.get(v);
	}
/*	public String decodeLiteral(int v) {
		return (v < 0 ? "-" : "") + dictionary.get(Math.abs(v));
	}
*/	
	public Var decodeVariable(int v) {
		return dictionary.get(Math.abs(v));
	}


	public static void main(String args[]) throws java.io.IOException, java.io.FileNotFoundException {
		SATModelDecoder smd = new SATModelDecoder(args);
		smd.clo.addFlag("trueonly", "Shows only variables that are true in the model");
		
		smd.run();
		
		if (!smd.isSAT()) {
			System.out.println("UNSATISFIABLE");
			return;
		}
	
		boolean trueonly = smd.isFlagSet("trueonly");
		for (int i=1; i<=smd.getMaxVar(); i++) {
			Boolean value = smd.getModelValue(i);
			Var var = smd.decodeVariable(i);
			if (smd.toHide.contains(var.family)) continue;
			if (trueonly) {
				if (value == null || !value) continue;
				System.out.println(var);
			} else {
				System.out.println(var + ": " + 
					(value == null ? " **any value**" : value));
			}
		}	
	}



	private List<Var> parseDictionary(String cnfFname) throws IOException {
		BufferedReader in = new BufferedReader(new FileReader(cnfFname));
		List<Var> dictionary = new ArrayList<Var>();
		int dictionarySize = 0;
		
		String line = "";
		boolean inDictionary = false;
		while ((line=in.readLine()) != null) {
			if (line.startsWith("c START DICTIONARY")) {
				inDictionary = true;
			} else if (inDictionary && line.startsWith("c END DICTIONARY")) {
					inDictionary = false;
					break;
			} else if (inDictionary) {
				// Parse dictionary line
				StringTokenizer tok = new StringTokenizer(line, "=");
				String str = tok.nextToken().substring(2);
				int value = Integer.parseInt(tok.nextToken());
				
				while (value >= dictionarySize) {
					dictionary.add(null);
					dictionarySize++;
				}
				dictionary.set(value, new Var(str));
			}			
		}
		in.close();
		return dictionary;
	}



	private List<Boolean> zChaff_parseModel(int nvars, String modelFname) throws FileNotFoundException, IOException {
		BufferedReader in = new BufferedReader(new FileReader(modelFname));
		String line = "";
		boolean sat = false;
		while ((line=in.readLine()) != null) {
			if (line.startsWith("Instance Satisfiable")) {
				sat = true;
				break;
			}
		}
		if (!sat) return null;
		// next line contains model
		return parseModelAsLine(nvars, in.readLine());
	}
	
	
	private List<Boolean> minisat_parseModel(int nvars, String modelFname) throws FileNotFoundException, IOException {
		BufferedReader in = new BufferedReader(new FileReader(modelFname));
		String line = "";
		boolean sat = false;
		while ((line=in.readLine()) != null) {
			if (line.startsWith("SAT")) {
				sat = true;
				break;
			}
		}
		if (!sat) return null;
		// next line contains model
		return parseModelAsLine(nvars, in.readLine());
	}	
	
	
	// Reads model in the format: "[-]1 [-]2 .... [-]N"
	private List<Boolean> parseModelAsLine(int nvars, String line) {
		List<Boolean> result = new ArrayList<Boolean>(nvars+1);
		for (int i=0; i<nvars+1; i++) result.add(null);
		Scanner s = new Scanner(line);
		while(s.hasNextInt()) {
			int lit = s.nextInt();
			result.set( (lit<0) ? -lit : lit, (lit>0) ? true : false);
		}
		return result;
	}


	private List<Boolean> picosat_parseModel(int nvars, String modelFname) throws FileNotFoundException, IOException {
		BufferedReader in = new BufferedReader(new FileReader(modelFname));
		String line = "";
		boolean sat = false;
		
		// Read first line
		line=in.readLine();
		if (line == null) throw new IOException("Could not read first line of the output file");

		if (line.equals("s SATISFIABLE")) {
				sat = true;
		}

		if (!sat) return null;
		// next lines contain the model. Each line starts with v
		List<Boolean> result = new ArrayList<Boolean>(nvars+1);
		for (int i=0; i<nvars+1; i++) result.add(null);

		while ((line=in.readLine()) != null) {
			if (!line.startsWith("v ")) {
				throw new IOException("Could not parse line \"" + line + "\"");
			}
			Scanner s = new Scanner(line);			
			s.next("v");
			while(s.hasNextInt()) {
				int lit = s.nextInt();
				result.set( (lit<0) ? -lit : lit, (lit>0) ? true : false);
			}
		}
		return result;
	}	
	
} //:~
