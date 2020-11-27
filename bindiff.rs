use std::fs::File;
use std::io::{ErrorKind, Read};

const LINELENGTH: usize = 0x10;

fn fread(file: &mut File, mut buf: &mut [u8]) -> std::io::Result<usize>
{
  let mut read = 0;
  while !buf.is_empty()
  {
    match file.read(buf)
    {
      Ok(0) => break,
      Ok(n) =>
      {
        read += n;
        let tmp = buf;
        buf = &mut tmp[n..];
      },
      Err(ref e) if e.kind() == ErrorKind::Interrupted =>
      {},
      Err(e) => return Err(e),
    }
  }
  Ok(read)
}

fn file_open_error(file_number: usize, file_name: &String) -> String
{
  format!("Error opening file {}: \"{}\"", file_number, &file_name)
}

fn file_read_error(file_number: usize, error: &std::io::Error) -> String
{
  format!("Error reading file {}: {}", file_number, &error)
}

fn bindiff(file1_name: &String, file2_name: &String) -> Result<(), String>
{
  let mut file1 =
    File::open(&file1_name).map_err(|_e| file_open_error(1, &file1_name))?;
  let mut file2 =
    File::open(&file2_name).map_err(|_e| file_open_error(2, &file2_name))?;

  println!("A: \x1B[32m{}\x1B[0m", &file1_name);
  println!("B: \x1B[31m{}\x1B[0m", &file2_name);

  let mut bytes1 = [0; LINELENGTH];
  let mut bytes2 = [0; LINELENGTH];

  let mut line = 0usize;

  let print_diff = |a: &u8, b: &u8| {
    print!("{}{:02X}\x1B[0m", if a == b { "\x1B[2m" } else { "\x1B[1m" }, a);
  };

  loop
  {
    let read1 =
      fread(&mut file1, &mut bytes1).map_err(|e| file_read_error(1, &e))?;
    let read2 =
      fread(&mut file2, &mut bytes2).map_err(|e| file_read_error(2, &e))?;
    let read = std::cmp::min(read1, read2);

    if read == 0usize
    {
      break;
    }

    let mut diff = false;
    for i in 0..read
    {
      if bytes1[i] != bytes2[i]
      {
        diff = true;
        break;
      }
    }

    if diff
    {
      // Print green line (file 1).
      print!("\n{:08X}", line);

      for (a, b) in bytes1[..read].iter().zip(&bytes2)
      {
        print!(" \x1B[32m");
        print_diff(a, b);
      }

      // Print red line (file 2).
      print!("\n{:08X}", line);

      for (a, b) in bytes1[..read].iter().zip(&bytes2)
      {
        print!(" \x1B[31m");
        print_diff(b, a);
      }

      println!("");
    }

    if read < LINELENGTH
    {
      break;
    }

    line += LINELENGTH;
  }

  Ok(())
}

fn error(str: &String)
{
  println!("\x1B[31m\x1B[1m{}\x1B[0m", &str);
  std::process::exit(1);
}

fn main()
{
  let argv: Vec<String> = std::env::args().collect();

  if argv.len() < 3
  {
    error(&"Too few argument".to_string());
  }

  print!("\x1B[0m");
  match bindiff(&argv[1], &argv[2])
  {
    Ok(()) => print!("\x1B[0m"),
    Err(str) => error(&str),
  };
}
