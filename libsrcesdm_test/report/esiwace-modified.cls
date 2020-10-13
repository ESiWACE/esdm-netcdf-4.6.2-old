\ProvidesClass{esiwace-modified}[2017/03/06 (BNL))]
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}% or whatever
\ProcessOptions\relax
%
\LoadClass[a4paper,11pt,enabledeprecatedfontcommands]{scrreprt}

%\RequirePackage{xcolors}
%\RequirePackage{...}

\usepackage[utf8]{inputenc}
\usepackage[usenames,dvipsnames,svgnames,table]{xcolor}
\usepackage[english]{babel}
\usepackage{geometry}
\newgeometry{left=3cm,bottom=2.5cm,right=2.5cm,top=2.5cm}

\bibliographystyle{alpha}

\usepackage[cmex10]{amsmath} % cmex10 forces type 1 font (no formula rasterization)
\usepackage{amssymb}
\usepackage{mathtools}
\usepackage{amsthm}
\usepackage{thmtools}

\usepackage{enumitem}
\usepackage{titling}
\usepackage{subcaption}
\usepackage{hyperref}
\usepackage{graphicx}
\usepackage{cleveref}
\usepackage{fancyhdr}
\usepackage{lastpage}
\usepackage{glossaries}
\usepackage{array, booktabs}
\usepackage{longtable, tabu}
\usepackage[table]{xcolor}
\usepackage{multirow}

\renewcommand{\ttdefault}{lmtt}

\definecolor{EsiOrange}{HTML}{F39600}
\definecolor{EsiBlue}{HTML}{006d9a}
\definecolor{EsiBlue2}{HTML}{006d9a}
\definecolor{EsiGreen}{HTML}{9ac013}
\definecolor{EsiGreen2}{HTML}{007000}

\hypersetup{
    colorlinks,
    linkcolor=EsiBlue,
    citecolor=EsiOrange,
    urlcolor=EsiGreen
}

\usepackage{listings}
\lstset{postbreak=\raisebox{0ex}[0ex][0ex]{\ensuremath{\color{blue}\hookrightarrow\space}},
%\lstdefinestyle{sig}{ %
  %backgroundcolor=\color{white},   % choose the background color; you must add \usepackage{color} or \usepackage{xcolor}; should come as last argument
  basicstyle=\fontsize{9}{8}\ttfamily,        % the size of the fonts that are used for the code
  breakatwhitespace=false,         % sets if automatic breaks should only happen at whitespace
  breaklines=true,                 % sets automatic line breaking
  captionpos=n,                    % sets the caption-position to bottom
  %commentstyle=\color{mygreen},    % comment style
  %deletekeywords={...},            % if you want to delete keywords from the given language
  %escapeinside={\%*}{*)},          % if you want to add LaTeX within your code
  extendedchars=true,              % lets you use non-ASCII characters; for 8-bits encodings only, does not work with UTF-8
  frame=none,	                   % adds a frame around the code frame={bt},
  keepspaces=true,                 % keeps spaces in text, useful for keeping indentation of code (possibly needs columns=flexible)
  keywordstyle=\bf\ttfamily,       % keyword style
  %language=C,                 % the language of the code
  %morekeywords={*,...},           % if you want to add more keywords to the set
  numbers=left,                    % where to put the line-numbers; possible values are (none, left, right)
  xleftmargin=4ex,
  numbersep=2.5ex,                   % how far the line-numbers are from the code
  numberstyle=\tiny\color{darkgray}, % the style that is used for the line-numbers
  rulecolor=\color{black},         % if not set, the frame-color may be changed on line-breaks within not-black text (e.g. comments (green here))
  showspaces=false,                % show spaces everywhere adding particular underscores; it overrides 'showstringspaces'
  showstringspaces=false,          % underline spaces within strings only
  showtabs=false,                  % show tabs within strings adding particular underscores
  stepnumber=1,                    % the step between two line-numbers. If it's 1, each line will be numbered
  %stringstyle=\color{mymauve},     % string literal style
  tabsize=2,	                   % sets default tabsize to 2 spaces
  title=\lstname                   % show the filename of files included with \lstinputlisting; also try caption instead of title
}

\lstset{language=C++,
    keywordstyle=\color{EsiGreen2}\ttfamily,
    stringstyle=\color{red}\ttfamily,
    commentstyle=\color{EsiBlue2}\ttfamily,
    morecomment=[l][\color{magenta}]{\#}
}

\usepackage{forloop}
\usepackage{tcolorbox}
\usepackage{colortbl}
\usepackage{totcount}
\usepackage{tikz}
\usepackage{pgfplots}

\setlength\headheight{18pt}

\tcbuselibrary{most}

\usetikzlibrary{shapes,arrows,decorations.pathmorphing,shapes.multipart,trees,positioning,shadows,shadows.blur}
\pgfdeclarelayer{background}
\pgfdeclarelayer{foreground}
\pgfsetlayers{background,main,foreground}


%
% New tools added by BNL to handle actions and discussions within document
% Overloading Julian's todo system

\newtotcounter{todocounter}
\newcounter{todocounterCur}
\newtotcounter{actioncounter}
\newcounter{actioncounterCur}

\renewcommand{\listtheoremname}{}
\newcommand{\listactions}{\listoftheorems[ignoreall, show=Action]}

\newcommand{\incrementtodocounter}{%
\stepcounter{todocounter}%
\stepcounter{todocounterCur}
\label{todo:\arabic{todocounterCur}}
}

\newcommand{\incrementactioncounter}{%
\stepcounter{actioncounter}
\stepcounter{actioncounterCur}
}

\declaretheoremstyle[
    spaceabove=6pt, spacebelow=6pt,
    headfont=\normalfont\bfseries,
    notefont=\mdseries, notebraces={---\ }{.},
    bodyfont=\normalfont, headpunct={},
    headformat={\NAME\ \NUMBER:\ \NOTE\\}
]{todostyle}

\declaretheorem[numberwithin=section,style=todostyle,
   shaded={bgcolor=Bisque},
   postheadhook=\incrementtodocounter,
   ]{Discussion}

\declaretheorem[style=todostyle,
    postheadhook=\incrementactioncounter,
    shaded={bgcolor=Azure}]{Action}
\newcommand{\action}[2]{
    \begin{Action}[#1]#2\end{Action}
    }



%
% end of tools for actions and discussions
%

\newtcolorbox[
	use counter=lstlisting,
	list inside=lol,
	list type=lstlisting
]
{tcbcode}[2][] {
	enhanced,
	fuzzy shadow={1mm}{-1mm}{0mm}{0.3mm}{EsiOrange!50!white},
	enlarge bottom by=3mm,
	enlarge top by=3mm,
	size=small,
	colframe=EsiOrange,
	coltitle=black,
	colback=black!0,
	title=Listing~\thechapter.\thetcbcounter:~#2,#1
}


\newcommand{\todo}[1]{
%\leavevmode \\
%\begin{tcolorbox}[colback=red!20!white,colframe=red!40!white]
%\textbf{TODO}: #1
%\end{tcolorbox}
%\stepcounter{todocounter}
%\stepcounter{todocounterCur}
%\label{todo:\arabic{todocounterCur}}
\begin{Discussion}#1\end{Discussion}
}

\crefname{lstlisting}{listing}{listing}
\Crefname{lstlisting}{Listing}{Listing}


\graphicspath{{./figures/}{../../template/images/}{./images/}{./assets/}{../assets/}}

\newenvironment{chapterIntro}{
  \begin{itshape}
}{
  \end{itshape}
}


\newtoks\ESIWACEtask
\newtoks\ESIWACElead
\newtoks\ESIWACEparticipants
\newtoks\ESIWACEworkpackage

%\renewcommand*{\maketitle}{%
%  \newpage
%  \pagestyle{myempty}
%  \begin{minipage}{\linewidth}
%      \null
%      \flushright
%      \rule{16cm}{5pt}\vskip1cm
%      \Huge{\@title}\\
%      \vspace{1cm}
%      for\\
%      \vspace{1cm}
%      ESIWACE\\
%      \vspace{1cm}
%      \LARGE{Release \myversion\\}
%      \LARGE{\@date}\\
%      \LARGE{Approved: \approvaldate\\}
%      \vspace{1cm}
%      \@author\\
%      \vspace{1cm}
%      \rule{16cm}{5pt}
%      \vfill
%  \end{minipage}
%  \newpage
%  \ifthenelse{\totvalue{todocounter}>0 \OR \totvalue{actioncounter}>0}{
%      \newpage
%      \ifthenelse{\totvalue{actioncounter}>0}
%        {\begin{tcolorbox}[colback=red!20!white,colframe=orange,
%        title=ACTION ITEMS]
%        \listactions
%        \end{tcolorbox}\vfill}
%      {}
%      %\ifthenelse{\totvalue{todocounter}>0 \AND
%%        \totvalue{actioncounter}>0}{\vfill}{}
%      \ifthenelse{\totvalue{todocounter}>0}{
%          \begin{tcolorbox}[colback=red!20!white,colframe=orange,title=TODOs]
%          \textbf{Discussion Items Remaining}: \total{todocounter}.\\ On pages
%          \newcounter{todoNum}
%          \forloop{todoNum}{1}{\value{todoNum} < \totvalue{todocounter}}{
%            %\arabic{todoNum}: page
%            - \pageref{todo:\arabic{todoNum}}
%          }
%          \end{tcolorbox}\vfill}
%      {}
%  }{}
%  \pagestyle{plain}
%}



\renewcommand*{\maketitle}{%
	\newpage
	\pagestyle{myempty}
	\null
	\vskip 2em%
	\begin{center}%
		\includegraphics[width=12cm]{esiwace-logo}
		\vspace*{2.5cm}

		\let \footnote \thanks
		{\huge \@title \par}%
		\vskip 1.5em%
		{\large
			\lineskip .5em%
			\begin{tabular}[t]{c}%
				\@author
			\end{tabular}\par}%
	\end{center}%

	\ifthenelse{\totvalue{todocounter}>1}{
		\begin{tcolorbox}[colback=red!20!white,colframe=orange,title=TODOs]
			\textbf{TODOs}: \total{todocounter}
			\newcounter{todoNum}
			\forloop{todoNum}{1}{\value{todoNum} < \totvalue{todocounter}}{
				%\arabic{todoNum}: page
				- \pageref{todo:\arabic{todoNum}}
			}
		\end{tcolorbox}
	}{}


	\vfill
	\hspace*{-0.9cm}
	\begin{minipage}{\textwidth}
		\large
		\begin{tabular}[t]{lm{9cm}}%
			%\hline
			Work Package: & Work Package 4 Exploitability \\ & \\
			Responsible Institution: & University of Reading (UREAD) \\ & \\
			Contributing Institutions:  & $\cdot$ Barcelona Supercomputing Center (BSC) \\
                                  & $\cdot$ Centre Européen de Recherche et de Formation Avancée en Calcul Scientifique (Cerfacs) \\
                                  & $\cdot$ Centro Euro-Mediterranean sui Cambiamenti Climatici (CMCC)\\
                                  & $\cdot$ Federal Office of Meteorology and Climatology (MeteoSwiss)\\
                                  & $\cdot$ DataDirect Networks (DDN)\\
                                  & $\cdot$ Deutsches Klimarechenzentrum GmbH (DKRZ)\\
                                  & $\cdot$ European Centre for Medium-Range Weather Forecasts (ECMWF)\\
                                  & $\cdot$ National Centre for Atmospheric Science (NCAS)\\
                                  & $\cdot$ Science and Technology Facilities Council (STFC)\\
                                  & $\cdot$ Seagate Systems UK Limited\\
                                      \\ & \\
                                   Date of Submission: & \today \\
			%\hline
		\end{tabular}
	\end{minipage}

	%\vspace*{1cm}
	%\hspace*{-0.6cm}
	%\begin{minipage}{11cm}
	%\tableofcontents
	%\end{minipage}

	\vspace*{1.5cm}

	\disclaimer

	\newpage
	\pagestyle{plain}
}





\newcommand{\fancyRulers}{
\renewcommand\footrule{\begin{minipage}{1\textwidth}
  \begin{tikzpicture}
    \path [left color = EsiOrange!100, right color = EsiGreen!100]
      (0,0) rectangle (\hsize,0.65ex);
    \path [left color = white, right color = white] (0,0) rectangle (\hsize,0.45ex);
    \path [left color = black, right color = black] (0,0) rectangle (\hsize,0.15ex);
  \end{tikzpicture}
\end{minipage}\par}%
\renewcommand\headrule{
\vspace*{-0.5em}
  \begin{tikzpicture}
    %\path [left color = black, right color = black] (0,0) rectangle (\hsize,0.6ex);
    %\path [left color = white, right color = white] (0,0) rectangle (\hsize,0.45ex);
    \path [left color = EsiOrange!100, right color = EsiGreen!100] (0,0) rectangle (\hsize,0.25ex);
  \end{tikzpicture}}%
}



\newcommand{\emptyRulers}{
\renewcommand\headrule{}
\renewcommand\footrule{}
}


\fancypagestyle{plain}{
  \fancyRulers
  \fancyhead[L]{}
  \fancyhead[R]{\leftmark}
  \fancyfoot[C]{}
  \fancyfoot[R]{\thepage/\pageref*{LastPage}}
  \fancyfoot[L]{\thetitle}
}

\fancypagestyle{empty}{
  \fancyRulers
  \fancyhead[L]{}
  \fancyhead[R]{}
  \fancyfoot[C]{}
  \fancyfoot[R]{}
  \fancyfoot[L]{}
}

\fancypagestyle{headings}{
  \fancyRulers
  \fancyhead[L]{}
  \fancyhead[R]{\leftmark}
  \fancyfoot[C]{}
  \fancyfoot[R]{\thepage\ of \pageref{LastPage}}
  \fancyfoot[L]{\thetitle\ (INTERNAL DRAFT)}
}


\fancypagestyle{myempty}{
  \emptyRulers
  \fancyhead[L]{}
  \fancyhead[R]{}
  \fancyfoot[C]{}
  \fancyfoot[R]{}
  \fancyfoot[L]{}
}


\newcommand{\cellHeader}[0]{\cellcolor{EsiOrange}}

\usepackage{tabu}

\taburulecolor{EsiOrange}

\renewcommand{\headrulewidth}{0.4pt}
\renewcommand{\footrulewidth}{0.4pt}

\newcommand{\ra}[1]{\renewcommand{\arraystretch}{#1}}
\newcolumntype{M}[1]{>{\centering\arraybackslash}m{#1}}

\renewcommand{\comment}[1]{}



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EU Commission Disclaimer
% See: http://ec.europa.eu/ipg/basics/legal/notice_copyright/notices_disclaimers/index_en.htm
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\newcommand{\disclaimer}{%
\noindent
\textit{\normalsize The information and views set out in this report are those of the author(s) and do not necessarily reflect the official opinion of the European Union. Neither the European Union institutions and bodies nor any person acting on their behalf may be held responsible for the use which may be made of the information contained therein.}
%Disclaimer: This material reflects only the author's view and the EU-Commission is not responsible for any use that may be made of the information it contains.}%
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EU H2020 Acknowledgement + Grant No
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\newcommand{\acknowledgement}[0]{
\section*{Acknowledgement}
The ESiWACE project has received funding from the European Union's Horizon 2020 research and innovation programme under grant agreement No \textbf{675191}

\medskip

\includegraphics[height=1.6cm]{eu-flag} \hspace*{2cm}
\includegraphics[height=1.6cm]{esiwace-logo}

\medskip
%
\disclaimer
}


%\makeglossaries



\endinput

% vim: set filetype=tex:
