package it.uniroma1.di.tmancini.utils;

import java.util.*;
import java.io.*;

public class CmdLineOptions {

	private Map<String, String> options = new HashMap<String, String>();
	private Map<String, String> optDescr = new HashMap<String, String>();
	private Set<String> mandatory = new HashSet<String>();
	private Set<String> flags = new HashSet<String>();	
	
	private String progName;
	private String version;
	private String author;
	private String descr;
	public CmdLineOptions(String progName, String version, String author, String descr) {
		this.progName = progName;
		this.version = version;
		this.author = author;
		this.descr = descr;
	}		
	
	public void addFlag(String name, String descr) {
		addOption(name, descr, false);
		flags.add(name);
	}
	
	public void addOption(String name, String descr, boolean isMandatory) {
		if (options.containsKey(name)) throw new Error("Cannot add same option twice: " + name);
		options.put(name, null);
		optDescr.put(name, descr);
		if (isMandatory) mandatory.add(name);
	}
	public void addOption(String name, String descr) {
		addOption(name, descr, false);	
	}
	
	public void parse(String args[]) {
		for(int i=0; i < args.length; i++) {
			if (!args[i].startsWith("--")) { 
				throw new Error("Unknown option/flag " + args[i]);
			}
			String opt = args[i].substring(2);
			if (!flags.contains(opt)) {
				// opt is an option: parse value
				String value = (i+1 < args.length ? args[++i] : null);
				setOptionValue(opt, value);
			} else {
				setFlag(opt);
			}
			
		}
		
		// Check whether all mandatory options are given a value
		for(String m : mandatory) {
			if (options.get(m) == null) {
				System.out.println("Error: Mandatory option/flag not given: " + m);
				printSyntax();
				return;
			}
		}
	}
		
	private void printSyntax() {
		System.out.println(progName + " version " + version + ", by " + author);
		System.out.println(descr);
		
		System.out.print("\nSyntax: ");
		System.out.print( ((progName != null && progName.length() > 0) ? progName : "ProgramName"));
		System.out.println(" <flags> <options>");
		
		System.out.println("\nFlags:");
		for(String f : flags) {
			System.out.print(" --" + f + " " + (mandatory.contains(f) ? " (mandatory)" : ""));
			System.out.print(": ");
			System.out.println(optDescr.get(f));
		}
		if (flags.isEmpty()) System.out.println("  none.");		
		
		System.out.println("\nOptions:");
		for(String f : options.keySet()) {
			if (flags.contains(f)) continue;
			System.out.print(" --" + f + " " + (mandatory.contains(f) ? " (mandatory)" : ""));
			System.out.print(": ");
			System.out.println(optDescr.get(f));
		}
		if (options.isEmpty()) System.out.println("  none.");
	}

	public String getOptionValue(String o) {
		return options.get(o);
	}
	
	public void setOptionValue(String o, String v) {
		if (!options.containsKey(o)) {
			printSyntax();
			throw new Error("Unrecognized option/flag " + o);
		}
		options.put(o,v);
	}
	
	public boolean isFlagSet(String f) {
		return options.get(f) != null;
	}
	public void setFlag(String f) {
		setOptionValue(f, "");
	}

} //:~