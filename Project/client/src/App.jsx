import { BrowserRouter as Router, Routes, Route } from 'react-router-dom'
import { Box } from '@mui/material'
import './App.css'
import Home from './Pages/Home.jsx'
import Profile from './Pages/Profile.jsx'
import NavBar from './Components/NavBar.jsx'
import Login from './Pages/Login.jsx'
import Register from './Pages/Register.jsx'
import BoardPage from './Pages/BoardPage.jsx'
import { GlobalProvider, useGlobal } from './SessionManager.jsx'
import { setAuthToken } from './APIManager.js'

function SetAuth(){
  const { state } = useGlobal();
  setAuthToken(state.authToken);
  return (<div></div>);
}

function App() {
  return (
    <GlobalProvider>
      <Router>
        <Box
          sx={{
            minHeight: '100vh',
            display: 'flex',
            flexDirection: 'column',
            backgroundColor: 'background.default',
          }}
        >
          <SetAuth />
          <NavBar />
          <Box
            component="main"
            sx={{
              flex: 1,
              width: '100%',
              py: 0,
            }}
          >
            <Routes>
              <Route path="/" element={<Home />} />
              <Route path="/profile" element={<Profile />} />
              <Route path="/login" element={<Login />} />
              <Route path="/register" element={<Register />} />
              <Route path="/board" element={<BoardPage />} />
            </Routes>
          </Box>
        </Box>
      </Router>
    </GlobalProvider>
  )
}

export default App