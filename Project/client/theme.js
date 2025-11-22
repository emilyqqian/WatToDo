import { createTheme } from '@mui/material/styles'

// NOTE: To load the pixel font, add this line to your public/index.html inside <head>:
// <link href="https://fonts.googleapis.com/css2?family=Press+Start+2P&display=swap" rel="stylesheet">

const theme = createTheme({
  palette: {
    mode: 'light',
    primary: {
      main: '#667eea',
      light: '#8b9dff',
      dark: '#4c63d2',
    },
    secondary: {
      main: '#764ba2',
      light: '#9b7bba',
      dark: '#5a3a80',
    },
    success: {
      main: '#10b981',
    },
    warning: {
      main: '#f59e0b',
    },
    error: {
      main: '#ef4444',
    },
    background: {
      default: '#f3f4f6',
      paper: '#ffffff',
    },
    text: {
      primary: '#ffffffff',
      secondary: '#6b7280',
    },
  },
  typography: {
    fontFamily: [
      'Inter',
      '-apple-system',
      'BlinkMacSystemFont',
      '"Segoe UI"',
      'sans-serif',
    ].join(','),
    h3: {
      fontWeight: 700,
      letterSpacing: '-0.02em',
    },
    h4: {
      // Use a blocky / pixel font for h4 headings (e.g. "Welcome back")
      fontFamily: ['"Press Start 2P"', 'Inter', 'sans-serif'].join(','),
      fontWeight: 700,
      fontSize: '1.25rem',
      letterSpacing: '-0.01em',
    },
    h6: {
      fontWeight: 600,
    },
  },
  components: {
    MuiButton: {
      styleOverrides: {
        contained: {
          textTransform: 'uppercase',
          fontWeight: 700,
          borderRadius: '0px',
          background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
          boxShadow: '4px 4px 0px rgba(0, 0, 0, 0.2)',
          border: '2px solid rgba(255, 255, 255, 0.3)',
          '&:hover': {
            background: 'linear-gradient(135deg, #5568d3 0%, #653a91 100%)',
            transform: 'translate(-2px, -2px)',
            boxShadow: '6px 6px 0px rgba(0, 0, 0, 0.2)',
          },
        },
      },
    },
    MuiCard: {
      styleOverrides: {
        root: {
          borderRadius: '0px',
          border: '3px solid rgba(255, 255, 255, 0.2)',
          boxShadow: '4px 4px 0px rgba(0, 0, 0, 0.15)',
        },
      },
    },
  },
})

export default theme
